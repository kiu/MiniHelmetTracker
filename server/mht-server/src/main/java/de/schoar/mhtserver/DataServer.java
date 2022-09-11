package de.schoar.mhtserver;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.Calendar;
import java.util.TimeZone;

import javax.annotation.PostConstruct;
import javax.annotation.PreDestroy;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

@Service
public class DataServer extends Thread {

	public static enum LED {
		OFF, ON, AUTO;
	}

	Logger logger = LoggerFactory.getLogger(DataServer.class);

	private ServerSocket serverSocket;
	private boolean running = true;

	private LED adminLed = LED.AUTO;
	private int adminMode = 42;
	private int adminUnclean = 0;

	@Autowired
	protected Teams teams;

	@Autowired
	protected DataRetriever retriever;

	public void setAdminLed(LED adminLed) {
		this.adminLed = adminLed;
	}

	public void setAdminMode(int adminMode) {
		this.adminMode = adminMode;
	}

	public void setAdminUnclean(int adminUnclean) {
		this.adminUnclean = adminUnclean;
	}

	@PostConstruct
	public void init() {
		this.start();
	}

	@Override
	public void run() {
		logger.debug("Starting ServerSocket port: " + Config.PORT);
		try {
			serverSocket = new ServerSocket(Config.PORT);
		} catch (IOException e) {
			e.printStackTrace();
			return;
		}
		logger.info("ServerSocket successfully started at port: " + Config.PORT);

		while (running) {
			try {
				new CommThread(serverSocket.accept()).start();
			} catch (IOException e) {
				try {
					Thread.sleep(1000);
				} catch (InterruptedException e1) {
				}
			}
		}
	}

	@PreDestroy
	public void down() {
		running = false;
		logger.info("Shutting down...");
		try {
			serverSocket.close();
		} catch (IOException e) {
		}
	}

	private class CommThread extends Thread {
		private Socket clientSocket;
		private OutputStream out;
		private BufferedReader in;

		public CommThread(Socket socket) {
			this.clientSocket = socket;
			logger.debug("Socket opened: " + socket);
		}

		private void send(OutputStream os, int len, byte[] ba) throws InterruptedException, IOException {
			byte[] data = new byte[len];
			data[0] = ba[0];
			for (int i = 1; i < len - 1; i++) {
				data[i] = (byte) (ba[i] + 'A');
			}
			data[len - 1] = ba[len - 1];
			out.write(data);
			out.flush();
			Thread.sleep(500);
		}

		@SuppressWarnings("unused")
		private boolean shouldOn() {
			Calendar now = Calendar.getInstance(TimeZone.getTimeZone("Europe/Berlin"));
			int h = now.get(Calendar.HOUR_OF_DAY);

			if (Config.DARK > Config.LIGHT) {
				return h >= Config.LIGHT && h < Config.DARK;
			} else {
				return !(h >= Config.DARK && h < Config.LIGHT);
			}
		}

		public void run() {
			if (teams == null) {
				logger.warn("No teams available. Closing.");
				close();
				return;
			}

			byte admin[] = new byte[6];

			admin[0] = 'A';

			if (LED.AUTO.equals(adminLed)) {
				if (shouldOn()) {
					admin[1] = (byte) 1;
				} else {
					admin[1] = (byte) 0;
				}
			}
			if (LED.ON.equals(adminLed)) {
				admin[1] = (byte) 1;
			}
			if (LED.OFF.equals(adminLed)) {
				admin[1] = (byte) 0;
			}

			admin[2] = (byte) adminMode;
			adminMode = 42;

			admin[3] = (byte) adminUnclean;
			adminUnclean = 0;

			if (retriever.isUpdateFailed()) {
				admin[4] = (byte) 1;
			} else {
				admin[4] = (byte) 0;
			}

			admin[5] = '\n';

			try {
				out = clientSocket.getOutputStream();
				in = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));

				out.write("mht-server\n".getBytes());
				out.flush();
				Thread.sleep(500);

				String line = in.readLine();
				if (line.equals("V01")) {
					send(out, 6, admin);
					send(out, 72, teams.getDataLeague());
					send(out, 72, teams.getDataConference());
					send(out, 72, teams.getDataDivision());
					send(out, 72, teams.getDataPlayoffs());
				} else {
					logger.debug("Unknown input: " + line);
				}
			} catch (Exception e) {
			}

			close();
		}

		public void close() {
			try {
				clientSocket.close();
				out.close();
				in.close();
			} catch (IOException e) {
			}
		}
	}
}