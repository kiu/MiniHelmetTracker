package de.schoar.mhtserver;

import java.io.BufferedReader;
import java.io.ByteArrayInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.URL;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.Base64;
import java.util.Calendar;
import java.util.Locale;

import javax.annotation.PostConstruct;
import javax.annotation.PreDestroy;
import javax.net.ssl.HttpsURLConnection;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.xpath.XPath;
import javax.xml.xpath.XPathConstants;
import javax.xml.xpath.XPathExpressionException;
import javax.xml.xpath.XPathFactory;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.w3c.dom.Document;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

import de.schoar.mhtserver.Team.PLAYOFF;

@Service
public class DataRetriever extends Thread {

	Logger logger = LoggerFactory.getLogger(DataRetriever.class);

	private boolean running = true;

	private boolean updateFailed = true;

	@Autowired
	protected Teams teams;

	@PostConstruct
	public void init() {
		this.start();
	}

	@Override
	public void run() {
		logger.info("Starting data retriever...");
		while (running) {
			if (shouldOn()) {
				update();
			} else {
				logger.debug("Sleeping we are in the off hours.");
			}
			try {
				Thread.sleep(Config.UPDATE_INTERVAL);
			} catch (Exception e) {
			}
		}
	}

	@PreDestroy
	public void down() {
		running = false;
		logger.info("Shutting down...");
		try {
		} catch (Exception e) {
		}
	}

	public boolean isUpdateFailed() {
		return updateFailed;
	}

	@SuppressWarnings("unused")
	private boolean shouldOn() {
		Calendar now = Calendar.getInstance(Locale.GERMANY);
		int h = now.get(Calendar.HOUR_OF_DAY);

		if (Config.DARK > Config.LIGHT) {
			return h >= Config.LIGHT && h < Config.DARK;
		} else {
			return !(h >= Config.DARK && h < Config.LIGHT);
		}
	}

	private String fixScore(String score) {
		return String.format("%02d", Integer.valueOf(score));
	}

	private String xpathNodeOrEmpty(XPath xp, Node node, String needle) throws XPathExpressionException {
		Node result = (Node) xp.compile(needle).evaluate(node, XPathConstants.NODE);
		if (result == null) {
			return "";
		}
		return result.getTextContent();
	}

	private void setPlayoffIfNotKickedOut(Team team, PLAYOFF playoff) {
		if (team.getPlayoff().equals(PLAYOFF.KICKED_OUT)) {
			return;
		}
		team.setPlayoff(playoff);
	}

	private void update() {
		logger.debug("Updating data...");

		boolean success = true;

		Teams tmpteams = new Teams();

		NodeList nodes;

		nodes = downloadAndParse(true);
		if (nodes != null) {
			try {
				XPath xPath = XPathFactory.newInstance().newXPath();
				for (int i = 0; i < nodes.getLength(); i++) {
					String homeAbbr = xpathNodeOrEmpty(xPath, nodes.item(i), "schedule/homeTeam/abbreviation");
					String awayAbbr = xpathNodeOrEmpty(xPath, nodes.item(i), "schedule/awayTeam/abbreviation");

					Team home = tmpteams.getTeam(homeAbbr);
					Team away = tmpteams.getTeam(awayAbbr);

					if (home == null) {
						logger.warn("Could not find home team " + homeAbbr);
						success = false;
						continue;
					}

					if (away == null) {
						logger.warn("Could not find away team " + awayAbbr);
						success = false;
						continue;
					}

					String awayScore = xpathNodeOrEmpty(xPath, nodes.item(i), "score/awayScoreTotal");
					String homeScore = xpathNodeOrEmpty(xPath, nodes.item(i), "score/homeScoreTotal");

					Integer as = toInt(awayScore);
					Integer hs = toInt(homeScore);

					if (as == null || hs == null) {
						continue;
					}
					
					if (Config.COUNT_INCOMPLETE_GAMES) {
						if (hs == 0 && as == 0) {
							continue;
						}
					} else {
						String status = xpathNodeOrEmpty(xPath, nodes.item(i), "schedule/playedStatus");
						if (!"COMPLETED".equals(status)) {
							continue;
						}
					}

					if (hs > as) {
						home.gameWon();
						away.gameLost();
					}

					if (hs == as) {
						home.gameTied();
						away.gameTied();
					}

					if (hs < as) {
						home.gameLost();
						away.gameWon();
					}

					logger.debug("Regular Game: " + home.getAbbrFixed() + " (" + fixScore(homeScore) + ") vs. "
							+ away.getAbbrFixed() + " (" + fixScore(awayScore) + ")");
				}
			} catch (Exception e) {
				logger.warn("Failed to parse regular games", e);
				success = false;
			}
		} else {
			logger.warn("No list available for regular season.");
			success = false;
		}

		nodes = downloadAndParse(false);
		if (nodes != null) {
			try {
				XPath xPath = XPathFactory.newInstance().newXPath();
				for (int i = 0; i < nodes.getLength(); i++) {

					String homeAbbr = xpathNodeOrEmpty(xPath, nodes.item(i), "schedule/homeTeam/abbreviation");
					String awayAbbr = xpathNodeOrEmpty(xPath, nodes.item(i), "schedule/awayTeam/abbreviation");

					Team home = tmpteams.getTeam(homeAbbr);
					Team away = tmpteams.getTeam(awayAbbr);

					if (home == null) {
						logger.warn("Could not find home team " + homeAbbr);
						success = false;
						continue;
					}

					if (away == null) {
						logger.warn("Could not find away team " + awayAbbr);
						success = false;
						continue;
					}

					setPlayoffIfNotKickedOut(home, PLAYOFF.COMPETING);
					setPlayoffIfNotKickedOut(away, PLAYOFF.COMPETING);

					String awayScore = xpathNodeOrEmpty(xPath, nodes.item(i), "score/awayScoreTotal");
					String homeScore = xpathNodeOrEmpty(xPath, nodes.item(i), "score/homeScoreTotal");

					Integer as = toInt(awayScore);
					Integer hs = toInt(homeScore);

					if (as == null || hs == null) {
						continue;
					}

					if (Config.COUNT_INCOMPLETE_GAMES) {
						if (hs == 0 && as == 0) {
							continue;
						}
					} else {
						String status = xpathNodeOrEmpty(xPath, nodes.item(i), "schedule/playedStatus");
						if (!"COMPLETED".equals(status)) {
							continue;
						}
					}

					if (hs > as) {
						setPlayoffIfNotKickedOut(home, PLAYOFF.COMPETING);
						setPlayoffIfNotKickedOut(away, PLAYOFF.KICKED_OUT);
					}

					if (hs < as) {
						setPlayoffIfNotKickedOut(home, PLAYOFF.KICKED_OUT);
						setPlayoffIfNotKickedOut(away, PLAYOFF.COMPETING);
					}

					logger.debug("Playoff Game: " + home.getAbbrFixed() + " (" + fixScore(homeScore) + ") vs. "
							+ away.getAbbrFixed() + " (" + fixScore(awayScore) + ")");
				}
			} catch (Exception e) {
				logger.warn("Failed to parse playoff games", e);
				success = false;
			}
		} else {
			logger.warn("No list available for playoff season.");
			//success = false;
		}

		updateFailed = !success;
		teams.setTeams(tmpteams.getTeams());
		logger.debug("...done.");
	}

	private Integer toInt(String value) {
		if (value == null || value.trim().length() == 0) {
			return null;
		}
		try {
			return Integer.parseInt(value);
		} catch (NumberFormatException e) {
			return null;
		}
	}
	
	private NodeList downloadAndParse(boolean regular) {
		String xml = download(regular);
		if (xml == null) {
			return null;
		}

		try {
			logger.debug("Parsing games...");
			DocumentBuilderFactory builderFactory = DocumentBuilderFactory.newInstance();
			DocumentBuilder builder = builderFactory.newDocumentBuilder();
			Document xmlDocument = builder.parse(new ByteArrayInputStream(xml.getBytes()));
			XPath xPath = XPathFactory.newInstance().newXPath();
			String expression = "/americanFootballGames/games";
			return (NodeList) xPath.compile(expression).evaluate(xmlDocument, XPathConstants.NODESET);
		} catch (Exception e) {
			logger.warn("Failed to parse games", e);
			return null;
		}
	}

	private String buildURL(boolean regular) {
		// Pre September
		// https://api.mysportsfeeds.com/v2.1/pull/nfl/2021-2022-regular/games.xml?force=false
		// https://api.mysportsfeeds.com/v2.1/pull/nfl/2022-playoff/games.xml?force=false

		// Post September
		// https://api.mysportsfeeds.com/v2.1/pull/nfl/2022-2023-regular/games.xml?force=false
		// https://api.mysportsfeeds.com/v2.1/pull/nfl/2023-playoff/games.xml?force=false

		String url = "https://api.mysportsfeeds.com/v2.1/pull/nfl";

		Calendar now = Calendar.getInstance(Locale.GERMANY);
		Calendar sept = Calendar.getInstance(Locale.GERMANY);
		sept.set(now.get(Calendar.YEAR), 8, 1);

		int from = now.get(Calendar.YEAR);
		int to = from + 1;

		if (now.before(sept)) {
			from -= 1;
			to -= 1;
		}

		if (regular) {
			url += "/" + from + "-" + to + "-regular";
		} else {
			url += "/" + to + "-playoff";
		}

		url += "/games.xml?force=false";
		return url;
	}

	private String download(boolean regular) {
		if (Config.LOCAL_LOAD) {
			try {
				logger.warn("Using local file istead of the web request.");
				return new String(Files.readAllBytes(Paths.get(regular + ".xml")), StandardCharsets.UTF_8);
			} catch (IOException e) {
				logger.warn("Local file not available.", e);
				return null;
			}
		}

		URL url = null;
		try {
			url = new URL(buildURL(regular));
			logger.debug("Downloading: " + url);

			String encoding = Base64.getEncoder().encodeToString((Config.APIKEY + ":MYSPORTSFEEDS").getBytes());

			HttpsURLConnection connection = (HttpsURLConnection) url.openConnection();
			connection.setRequestMethod("GET");
			connection.setDoOutput(true);
			connection.setRequestProperty("Authorization", "Basic " + encoding);

			InputStream content = (InputStream) connection.getInputStream();
			BufferedReader in = new BufferedReader(new InputStreamReader(content));

			String line;
			StringBuffer sb = new StringBuffer();
			while ((line = in.readLine()) != null) {
				sb.append(line);
			}
			in.close();

			if (Config.LOCAL_SAVE) {
				logger.warn("Storing received data on local filesystem.");
				FileOutputStream fos = new FileOutputStream(regular + ".xml");
				fos.write(sb.toString().getBytes());
				fos.close();
			}

			return sb.toString();
		} catch (Exception e) {
			logger.warn("Failed to download: " + url, e);
			return null;
		}
	}
}
