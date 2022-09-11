package de.schoar.mhtserver;

public class Team {

	static enum PLAYOFF {
		NOT_QUALIFIED, KICKED_OUT, COMPETING
	}

	static enum CONFERENCE {
		AFC, NFC
	}

	static enum DIVISION {
		AFC_EAST, AFC_NORTH, AFC_SOUTH, AFC_WEST, NFC_EAST, NFC_NORTH, NFC_SOUTH, NFC_WEST;
	}

	private final Integer id;
	private final String abbr;
	private final CONFERENCE conference;
	private final DIVISION division;

	private int gamesPlayed = 0;
	private int gamesWon = 0;
	private int gamesTied = 0;
	@SuppressWarnings("unused")
	private int gamesLost = 0;

	private Double pct = -1.0;
	private PLAYOFF playoff = PLAYOFF.NOT_QUALIFIED;

	public Team(int id, String abbr, CONFERENCE conference, DIVISION division) {
		this.id = id;
		this.abbr = abbr;
		this.conference = conference;
		this.division = division;
	}

	public Integer getId() {
		return id;
	}

	public String getAbbr() {
		return abbr;
	}

	public String getAbbrFixed() {
		if (abbr.length() == 2) {
			return abbr + " ";
		}
		return abbr;
	}

	public int getGamesPlayed() {
		return gamesPlayed;
	}

	public CONFERENCE getConference() {
		return conference;
	}

	public DIVISION getDivision() {
		return division;
	}

	public Double getPCT() {
		return this.pct;
	}

	private void updatePCT() {
		if (gamesPlayed == 0) {
			this.pct = -1.0;
			return;
		}
		this.pct = ((double) gamesWon + (0.5 * (double) gamesTied)) / (double) gamesPlayed;
	}

	public PLAYOFF getPlayoff() {
		return playoff;
	}

	public void setPlayoff(PLAYOFF playoff) {
		this.playoff = playoff;
	}

	public void gameWon() {
		this.gamesPlayed++;
		this.gamesWon++;
		updatePCT();
	}

	public void gameTied() {
		this.gamesPlayed++;
		this.gamesTied++;
		updatePCT();
	}

	public void gameLost() {
		this.gamesPlayed++;
		this.gamesLost++;
		updatePCT();
	}

	@Override
	public String toString() {
		return "[" + String.format("%02d", id) + "] " + getAbbrFixed() + ": " + pct + " " + playoff.name();
	}

}
