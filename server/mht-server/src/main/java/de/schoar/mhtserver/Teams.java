package de.schoar.mhtserver;

import java.util.Collections;
import java.util.Comparator;
import java.util.LinkedList;
import java.util.List;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.stereotype.Component;

import de.schoar.mhtserver.Team.CONFERENCE;
import de.schoar.mhtserver.Team.DIVISION;
import de.schoar.mhtserver.Team.PLAYOFF;

@Component
public class Teams {

	Logger logger = LoggerFactory.getLogger(Teams.class);

	private List<Team> teams = new LinkedList<Team>();

	private byte data_league[] = new byte[72];
	private byte data_conference[] = new byte[72];
	private byte data_division[] = new byte[72];
	private byte data_playoffs[] = new byte[72];
	
	public Teams() {
		teams.add(new Team(1, "ARI", CONFERENCE.NFC, DIVISION.NFC_WEST));
		teams.add(new Team(2, "ATL", CONFERENCE.NFC, DIVISION.NFC_SOUTH));
		teams.add(new Team(3, "BAL", CONFERENCE.AFC, DIVISION.AFC_NORTH));
		teams.add(new Team(4, "BUF", CONFERENCE.AFC, DIVISION.AFC_EAST));
		teams.add(new Team(5, "CAR", CONFERENCE.NFC, DIVISION.NFC_SOUTH));
		teams.add(new Team(6, "CHI", CONFERENCE.NFC, DIVISION.NFC_NORTH));
		teams.add(new Team(7, "CIN", CONFERENCE.AFC, DIVISION.AFC_NORTH));
		teams.add(new Team(8, "CLE", CONFERENCE.AFC, DIVISION.AFC_NORTH));
		teams.add(new Team(9, "DAL", CONFERENCE.NFC, DIVISION.NFC_EAST));

		teams.add(new Team(10, "DEN", CONFERENCE.AFC, DIVISION.AFC_WEST));
		teams.add(new Team(11, "DET", CONFERENCE.NFC, DIVISION.NFC_NORTH));
		teams.add(new Team(12, "GB", CONFERENCE.NFC, DIVISION.NFC_NORTH));
		teams.add(new Team(13, "HOU", CONFERENCE.AFC, DIVISION.AFC_SOUTH));
		teams.add(new Team(14, "IND", CONFERENCE.AFC, DIVISION.AFC_SOUTH));
		teams.add(new Team(15, "JAX", CONFERENCE.AFC, DIVISION.AFC_SOUTH));
		teams.add(new Team(16, "KC", CONFERENCE.AFC, DIVISION.AFC_WEST));
		teams.add(new Team(17, "MIA", CONFERENCE.AFC, DIVISION.AFC_EAST));
		teams.add(new Team(18, "MIN", CONFERENCE.NFC, DIVISION.NFC_NORTH));
		teams.add(new Team(19, "NE", CONFERENCE.AFC, DIVISION.AFC_EAST));

		teams.add(new Team(20, "NO", CONFERENCE.NFC, DIVISION.NFC_SOUTH));
		teams.add(new Team(21, "NYG", CONFERENCE.NFC, DIVISION.NFC_EAST));
		teams.add(new Team(22, "NYJ", CONFERENCE.AFC, DIVISION.AFC_EAST));
		teams.add(new Team(23, "LV", CONFERENCE.AFC, DIVISION.AFC_WEST));
		teams.add(new Team(24, "PHI", CONFERENCE.NFC, DIVISION.NFC_EAST));
		teams.add(new Team(25, "PIT", CONFERENCE.AFC, DIVISION.AFC_NORTH));
		teams.add(new Team(26, "LAC", CONFERENCE.AFC, DIVISION.AFC_WEST));
		teams.add(new Team(27, "SF", CONFERENCE.NFC, DIVISION.NFC_WEST));
		teams.add(new Team(28, "SEA", CONFERENCE.NFC, DIVISION.NFC_WEST));
		teams.add(new Team(29, "LA", CONFERENCE.NFC, DIVISION.NFC_WEST));

		teams.add(new Team(30, "TB", CONFERENCE.NFC, DIVISION.NFC_SOUTH));
		teams.add(new Team(31, "TEN", CONFERENCE.AFC, DIVISION.AFC_SOUTH));
		teams.add(new Team(32, "WAS", CONFERENCE.NFC, DIVISION.NFC_EAST));

		updateRegular();
		updatePlayoffs();
	}

	public byte[] getDataLeague() {
		return data_league;
	}

	public byte[] getDataConference() {
		return data_conference;
	}

	public byte[] getDataDivision() {
		return data_division;
	}

	public byte[] getDataPlayoffs() {
		return data_playoffs;
	}
	
	public List<Team> getTeams() {
		return teams;
	}

	public void setTeams(List<Team> teams) {
		this.teams = teams;
		updateRegular();
		updatePlayoffs();
	}

	public Team getTeam(String abbr) {
		for (Team t : teams) {
			if (t.getAbbr().equals(abbr)) {
				return t;
			}
		}
		logger.warn("Didn't find shortname: " + abbr);
		return null;
	}

	public List<Team> getTeam(CONFERENCE league) {
		List<Team> filtered = new LinkedList<Team>();
		for (Team t : teams) {
			if (league.equals(t.getConference())) {
				filtered.add(t);
			}
		}
		return filtered;
	}

	public List<Team> getTeam(DIVISION division) {
		List<Team> filtered = new LinkedList<Team>();
		for (Team t : teams) {
			if (division.equals(t.getDivision())) {
				filtered.add(t);
			}
		}
		return filtered;
	}

	private void updateRegular() {
		for (int i = 0; i < 72; i++) {
			data_league[i] = 0;
		}
		data_league[0] = 'L';
		data_league[71] = '\n';

		Collections.sort(teams, new Comparator<Team>() {
			@Override
			public int compare(Team t1, Team t2) {
				int c = t2.getPCT().compareTo(t1.getPCT());
				if (c != 0) {
					return c;
				}
				return t1.getId().compareTo(t2.getId());
			}
		});

		int pos = 2;
		int color = 0;
		int matched = 0;

		double lastpct = 23.42;
		for (Team t : teams) {
			if (lastpct == t.getPCT()) {
				matched++;
			} else {
				color += matched;
				matched = 0;
				color++;
			}
			lastpct = t.getPCT();
			data_league[pos] = t.getId().byteValue();
			data_league[pos + 35] = (byte) color;
			pos++;
		}

		// -------------------------------------------------------------------------------------

		for (int i = 0; i < 72; i++) {
			data_conference[i] = 0;
		}
		data_conference[0] = 'C';
		data_conference[71] = '\n';

		Collections.sort(teams, new Comparator<Team>() {
			@Override
			public int compare(Team t1, Team t2) {
				int c = t1.getPCT().compareTo(t2.getPCT());
				if (c != 0) {
					return c;
				}
				return t1.getId().compareTo(t2.getId());
			}
		});

		pos = 2;
		color = 34;
		matched = 0;

		lastpct = 23.42;
		for (Team t : getTeam(CONFERENCE.AFC)) {
			if (lastpct == t.getPCT()) {
				matched++;
			} else {
				color -= (matched * 2);
				matched = 0;
				color -= 2;
			}
			lastpct = t.getPCT();
			data_conference[pos] = t.getId().byteValue();
			data_conference[pos + 35] = (byte) color;
			pos++;
		}

		pos++;

		Collections.sort(teams, new Comparator<Team>() {
			@Override
			public int compare(Team t1, Team t2) {
				int c = t2.getPCT().compareTo(t1.getPCT());
				if (c != 0) {
					return c;
				}
				return t2.getId().compareTo(t1.getId());
			}
		});

		color = 0;
		matched = 0;

		lastpct = 23.42;
		for (Team t : getTeam(CONFERENCE.NFC)) {
			if (lastpct == t.getPCT()) {
				matched++;
			} else {
				color += (matched * 2);
				matched = 0;
				color += 2;
			}
			lastpct = t.getPCT();
			data_conference[pos] = t.getId().byteValue();
			data_conference[pos + 35] = (byte) color;
			pos++;
		}

		// -------------------------------------------------------------------------------------

		for (int i = 0; i < 72; i++) {
			data_division[i] = 0;			
		}
		data_division[0] = 'D';
		data_division[71] = '\n';

		Collections.sort(teams, new Comparator<Team>() {
			@Override
			public int compare(Team t1, Team t2) {
				int c = t1.getPCT().compareTo(t2.getPCT());
				if (c != 0) {
					return c;
				}
				return t1.getId().compareTo(t2.getId());
			}
		});

		byte preset[] = new byte[] { 1, 18, 26, 32 };

		pos = 2;
		color = 4;
		matched = 0;

		lastpct = 23.42;
		for (Team t : getTeam(DIVISION.AFC_EAST)) {
			if (lastpct == t.getPCT()) {
				matched++;
			} else {
				color -= matched;
				matched = 0;
				color -= 1;
			}
			lastpct = t.getPCT();
			data_division[pos] = t.getId().byteValue();
			data_division[pos + 35] = preset[color];
			pos++;
		}

		color = 4;
		matched = 0;

		lastpct = 23.42;
		for (Team t : getTeam(DIVISION.AFC_NORTH)) {
			if (lastpct == t.getPCT()) {
				matched++;
			} else {
				color -= matched;
				matched = 0;
				color -= 1;
			}
			lastpct = t.getPCT();
			data_division[pos] = t.getId().byteValue();
			data_division[pos + 35] = preset[color];
			pos++;
		}

		color = 4;
		matched = 0;

		lastpct = 23.42;
		for (Team t : getTeam(DIVISION.AFC_SOUTH)) {
			if (lastpct == t.getPCT()) {
				matched++;
			} else {
				color -= matched;
				matched = 0;
				color -= 1;
			}
			lastpct = t.getPCT();
			data_division[pos] = t.getId().byteValue();
			data_division[pos + 35] = preset[color];
			pos++;
		}

		color = 4;
		matched = 0;

		lastpct = 23.42;
		for (Team t : getTeam(DIVISION.AFC_WEST)) {
			if (lastpct == t.getPCT()) {
				matched++;
			} else {
				color -= matched;
				matched = 0;
				color -= 1;
			}
			lastpct = t.getPCT();
			data_division[pos] = t.getId().byteValue();
			data_division[pos + 35] = preset[color];
			pos++;
		}

		pos++;

		Collections.sort(teams, new Comparator<Team>() {
			@Override
			public int compare(Team t1, Team t2) {
				int c = t2.getPCT().compareTo(t1.getPCT());
				if (c != 0) {
					return c;
				}
				return t2.getId().compareTo(t1.getId());
			}
		});

		color = -1;
		matched = 0;

		lastpct = 23.42;
		for (Team t : getTeam(DIVISION.NFC_WEST)) {
			if (lastpct == t.getPCT()) {
				matched++;
			} else {
				color += matched;
				matched = 0;
				color += 1;
			}
			lastpct = t.getPCT();
			data_division[pos] = t.getId().byteValue();
			data_division[pos + 35] = preset[color];
			pos++;
		}

		color = -1;
		matched = 0;

		lastpct = 23.42;
		for (Team t : getTeam(DIVISION.NFC_SOUTH)) {
			if (lastpct == t.getPCT()) {
				matched++;
			} else {
				color += matched;
				matched = 0;
				color += 1;
			}
			lastpct = t.getPCT();
			data_division[pos] = t.getId().byteValue();
			data_division[pos + 35] = preset[color];
			pos++;
		}

		color = -1;
		matched = 0;

		lastpct = 23.42;
		for (Team t : getTeam(DIVISION.NFC_NORTH)) {
			if (lastpct == t.getPCT()) {
				matched++;
			} else {
				color += matched;
				matched = 0;
				color += 1;
			}
			lastpct = t.getPCT();
			data_division[pos] = t.getId().byteValue();
			data_division[pos + 35] = preset[color];
			pos++;
		}

		color = -1;
		matched = 0;

		lastpct = 23.42;
		for (Team t : getTeam(DIVISION.NFC_EAST)) {
			if (lastpct == t.getPCT()) {
				matched++;
			} else {
				color += matched;
				matched = 0;
				color += 1;
			}
			lastpct = t.getPCT();
			data_division[pos] = t.getId().byteValue();
			data_division[pos + 35] = preset[color];
			pos++;
		}

	}
	
	private void updatePlayoffs() {
		Collections.sort(teams, new Comparator<Team>() {
			@Override
			public int compare(Team t1, Team t2) {
				return t1.getId().compareTo(t2.getId());
			}
		});

		for (int i = 0; i < 72; i++) {
			data_playoffs[i] = 0;
		}
		data_playoffs[0] = 'P';		
		data_playoffs[71] = '\n';		

		int pos = 1;

		for (Team t : getTeam(CONFERENCE.AFC)) {
			if (PLAYOFF.NOT_QUALIFIED.equals(t.getPlayoff())) {
				data_playoffs[pos] = t.getId().byteValue();
				data_playoffs[pos + 35] = 32;
				pos++;
			}
		}
		for (Team t : getTeam(CONFERENCE.AFC)) {
			if (PLAYOFF.KICKED_OUT.equals(t.getPlayoff())) {
				data_playoffs[pos] = t.getId().byteValue();
				data_playoffs[pos + 35] = 22;
				pos++;
			}
		}

		pos++;

		for (Team t : getTeam(CONFERENCE.AFC)) {
			if (PLAYOFF.COMPETING.equals(t.getPlayoff())) {
				data_playoffs[pos] = t.getId().byteValue();
				data_playoffs[pos + 35] = 1;
				pos++;
			}
		}

		pos++;

		Collections.sort(teams, new Comparator<Team>() {
			@Override
			public int compare(Team t1, Team t2) {
				return t2.getId().compareTo(t1.getId());
			}
		});

		for (Team t : getTeam(CONFERENCE.NFC)) {
			if (PLAYOFF.COMPETING.equals(t.getPlayoff())) {
				data_playoffs[pos] = t.getId().byteValue();
				data_playoffs[pos + 35] = 1;
				pos++;
			}
		}

		pos++;

		for (Team t : getTeam(CONFERENCE.NFC)) {
			if (PLAYOFF.KICKED_OUT.equals(t.getPlayoff())) {
				data_playoffs[pos] = t.getId().byteValue();
				data_playoffs[pos + 35] = 22;
				pos++;
			}
		}

		for (Team t : getTeam(CONFERENCE.NFC)) {
			if (PLAYOFF.NOT_QUALIFIED.equals(t.getPlayoff())) {
				data_playoffs[pos] = t.getId().byteValue();
				data_playoffs[pos + 35] = 32;
				pos++;
			}
		}
	}

}
