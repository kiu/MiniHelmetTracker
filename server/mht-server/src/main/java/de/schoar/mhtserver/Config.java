package de.schoar.mhtserver;

import java.util.Calendar;
import java.util.TimeZone;

public class Config {

	public static final String APIKEY = "secret";
	
	public static final long UPDATE_INTERVAL = 1000 * 60 * 10;

	public static final boolean COUNT_INCOMPLETE_GAMES = true;
	
	public static final boolean LOCAL_LOAD = false;
	public static final boolean LOCAL_SAVE = false;
	
	public static final int DARK = 23;
	public static final int LIGHT = 9;
	
	public static final int PORT = 2207;
		
	@SuppressWarnings("unused")
	public static boolean isAwake() {
		Calendar now = Calendar.getInstance(TimeZone.getTimeZone("Europe/Berlin"));
		int h = now.get(Calendar.HOUR_OF_DAY);

		if (Config.DARK > Config.LIGHT) {
			return h >= Config.LIGHT && h < Config.DARK;
		} else {
			return !(h >= Config.DARK && h < Config.LIGHT);
		}
	}
}
