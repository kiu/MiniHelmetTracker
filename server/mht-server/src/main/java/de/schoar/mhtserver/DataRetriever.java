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
import java.text.SimpleDateFormat;
import java.util.Base64;
import java.util.Calendar;
import java.util.Date;
import java.util.LinkedList;
import java.util.List;
import java.util.TimeZone;

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

	public static enum POLL {
		OFF, ON, AUTO;
	}

	private boolean running = true;

	private boolean updateFailed = true;

	private POLL adminPoll = POLL.AUTO;

	private boolean updateOnce = true;

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
			if (updateOnce) {
				updateOnce = false;
				update();
			} else if (POLL.ON.equals(adminPoll)) {
				update();
			} else if (POLL.AUTO.equals(adminPoll)) {
				if (Config.isAwake()) {
					update();
				} else {
					logger.debug("Sleeping. We are in the off hours.");
				}
			} else {
				logger.debug("Sleeping. Admin disabled polling.");
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
			notifyAll();
		} catch (Exception e) {
		}
	}

	public void setAdminPoll(POLL adminPoll) {
		this.adminPoll = adminPoll;
	}

	public boolean isUpdateFailed() {
		return updateFailed;
	}

	private void update() {
		logger.debug("Updating data...");

		// Pre September
		// https://api.mysportsfeeds.com/v2.1/pull/nfl/2021-2022-regular/games.xml?force=false
		// https://api.mysportsfeeds.com/v2.1/pull/nfl/2021-2022-regular/date/20210908/games.xml
		// https://api.mysportsfeeds.com/v2.1/pull/nfl/2022-playoff/games.xml?force=false
		// https://api.mysportsfeeds.com/v2.1/pull/nfl/2022-playoff/date/20220116/games.xml

		// Post September
		// https://api.mysportsfeeds.com/v2.1/pull/nfl/2022-2023-regular/games.xml?force=false
		// https://api.mysportsfeeds.com/v2.1/pull/nfl/2022-2023-regular/date/20220908/games.xml
		// https://api.mysportsfeeds.com/v2.1/pull/nfl/2023-playoff/games.xml?force=false
		// https://api.mysportsfeeds.com/v2.1/pull/nfl/2023-playoff/date/20220116/games.xml

		String base = "https://api.mysportsfeeds.com/v2.1/pull/nfl";
		String seasonRegular = urlSeason(true);
		String seasonPlayoff = urlSeason(false);
		String day = urlDate();

		boolean success = true;
		boolean result;

		Teams tmpteams = new Teams();

		List<String> ids = new LinkedList<String>();

		NodeList regularDay = downloadToNodelist(base + "/" + seasonRegular + "/date/" + day + "/games.xml?force=false",
				"mht-regular-day.xml");
		result = parseRegular(regularDay, tmpteams, ids);
		if (!result) {
			success = false;
		}

		NodeList regularAll = downloadToNodelist(base + "/" + seasonRegular + "/games.xml?force=false",
				"mht-regular-all.xml");
		result = parseRegular(regularAll, tmpteams, ids);
		if (!result) {
			success = false;
		}

		ids.clear();

		NodeList playoffDay = downloadToNodelist(base + "/" + seasonPlayoff + "/date/" + day + "/games.xml?force=false",
				"mht-playoff-day.xml");
		result = parsePlayoff(playoffDay, tmpteams, ids);
		if (!result) {
			success = false;
		}

		NodeList playoffAll = downloadToNodelist(base + "/" + seasonPlayoff + "/games.xml?force=false",
				"mht-playoff-all.xml");
		result = parsePlayoff(playoffAll, tmpteams, ids);
		if (!result) {
			success = false;
		}

		updateFailed = !success;
		teams.setTeams(tmpteams.getTeams());
		logger.debug("...done.");
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

	private boolean parseRegular(NodeList nodes, Teams tmpteams, List<String> ids) {
		if (nodes == null) {
			logger.warn("No list available for regular season.");
			return false;
		}
		if (nodes.getLength() == 0) {
			logger.debug("List is empty for regular season.");
			return true;

		}

		boolean success = true;

		try {
			XPath xPath = XPathFactory.newInstance().newXPath();
			for (int i = 0; i < nodes.getLength(); i++) {

				String id = xpathNodeOrEmpty(xPath, nodes.item(i), "schedule/id");
				if (id == null || id.trim().length() == 0) {
					logger.debug("Game without id.");
					continue;
				}
				if (ids.contains(id)) {
					continue;
				}
				ids.add(id);

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

				String status = xpathNodeOrEmpty(xPath, nodes.item(i), "schedule/playedStatus");
				if (Config.COUNT_INCOMPLETE_GAMES) {
					if ("UNPLAYED".equals(status)) {
						continue;
					}
				} else {
					if (!"COMPLETED".equals(status)) {
						continue;
					}
				}

				String awayScore = xpathNodeOrEmpty(xPath, nodes.item(i), "score/awayScoreTotal");
				String homeScore = xpathNodeOrEmpty(xPath, nodes.item(i), "score/homeScoreTotal");

				Integer as = toInt(awayScore);
				Integer hs = toInt(homeScore);

				if (as == null || hs == null) {
					continue;
				}

				if (hs.intValue() > as.intValue()) {
					home.gameWon();
					away.gameLost();
				}

				if (hs.intValue() == as.intValue()) {
					home.gameTied();
					away.gameTied();
				}

				if (hs.intValue() < as.intValue()) {
					home.gameLost();
					away.gameWon();
				}

				logger.debug("Regular Game: " + home.getAbbrFixed() + " (" + fixScore(homeScore) + ") vs. "
						+ away.getAbbrFixed() + " (" + fixScore(awayScore) + ")");
			}
		} catch (Exception e) {
			logger.warn("Failed to parse regular games", e);
			return false;
		}

		return success;
	}

	private boolean parsePlayoff(NodeList nodes, Teams tmpteams, List<String> ids) {
		if (nodes == null) {
			logger.warn("No list available for playoff season.");
			return true;
		}
		if (nodes.getLength() == 0) {
			logger.debug("List is empty for playoff season.");
			return true;

		}

		boolean success = true;

		try {
			XPath xPath = XPathFactory.newInstance().newXPath();
			for (int i = 0; i < nodes.getLength(); i++) {

				String id = xpathNodeOrEmpty(xPath, nodes.item(i), "schedule/id");
				if (id == null || id.trim().length() == 0) {
					logger.debug("Game without id.");
					continue;
				}
				if (ids.contains(id)) {
					continue;
				}
				ids.add(id);

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

				String status = xpathNodeOrEmpty(xPath, nodes.item(i), "schedule/playedStatus");
				if (Config.COUNT_INCOMPLETE_GAMES) {
					if ("UNPLAYED".equals(status)) {
						continue;
					}
				} else {
					if (!"COMPLETED".equals(status)) {
						continue;
					}
				}

				String awayScore = xpathNodeOrEmpty(xPath, nodes.item(i), "score/awayScoreTotal");
				String homeScore = xpathNodeOrEmpty(xPath, nodes.item(i), "score/homeScoreTotal");

				Integer as = toInt(awayScore);
				Integer hs = toInt(homeScore);

				if (as == null || hs == null) {
					continue;
				}

				if (hs.intValue() > as.intValue()) {
					setPlayoffIfNotKickedOut(home, PLAYOFF.COMPETING);
					setPlayoffIfNotKickedOut(away, PLAYOFF.KICKED_OUT);
				}

				if (hs.intValue() == as.intValue()) {
					setPlayoffIfNotKickedOut(home, PLAYOFF.COMPETING);
					setPlayoffIfNotKickedOut(away, PLAYOFF.COMPETING);
				}

				if (hs.intValue() < as.intValue()) {
					setPlayoffIfNotKickedOut(home, PLAYOFF.KICKED_OUT);
					setPlayoffIfNotKickedOut(away, PLAYOFF.COMPETING);
				}

				logger.debug("Playoff Game: " + home.getAbbrFixed() + " (" + fixScore(homeScore) + ") vs. "
						+ away.getAbbrFixed() + " (" + fixScore(awayScore) + ")");
			}
		} catch (Exception e) {
			logger.warn("Failed to parse playoff games", e);
			return false;
		}

		return success;
	}

	private NodeList downloadToNodelist(String url, String local) {
		String xml = download(url, local);

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

	private String urlDate() {
		Calendar now = Calendar.getInstance(TimeZone.getTimeZone("America/New_York"));
		if (now.get(Calendar.HOUR_OF_DAY) < 8) {
			now.roll(Calendar.DATE, false);
		}
		Date time = now.getTime();
		SimpleDateFormat format = new SimpleDateFormat("yyyyMMdd");
		return format.format(time);
	}

	private String urlSeason(boolean regular) {
		Calendar now = Calendar.getInstance(TimeZone.getTimeZone("Europe/Berlin"));
		Calendar sept = Calendar.getInstance(TimeZone.getTimeZone("Europe/Berlin"));
		sept.set(now.get(Calendar.YEAR), 8, 1);

		int from = now.get(Calendar.YEAR);
		int to = from + 1;

		if (now.before(sept)) {
			from -= 1;
			to -= 1;
		}

		if (regular) {
			return from + "-" + to + "-regular";
		} else {
			return to + "-playoff";
		}
	}

	private String download(String urlstr, String local) {
		if (Config.LOCAL_LOAD) {
			try {
				logger.warn("Using local file istead of the web request: " + local);
				return new String(Files.readAllBytes(Paths.get(local)), StandardCharsets.UTF_8);
			} catch (IOException e) {
				logger.warn("Local file not available.", e);
				return null;
			}
		}

		URL url = null;

		try {
			url = new URL(urlstr);
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
				logger.warn("Storing received data on local filesystem: " + local);
				FileOutputStream fos = new FileOutputStream(local);
				fos.write(sb.toString().getBytes());
				fos.close();
			}

			Thread.sleep(1000);

			return sb.toString();
		} catch (Exception e) {
			logger.warn("Failed to download: " + url);
			return null;
		}
	}
}
