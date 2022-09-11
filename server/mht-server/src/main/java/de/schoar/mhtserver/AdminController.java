package de.schoar.mhtserver;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.ExitCodeGenerator;
import org.springframework.boot.SpringApplication;
import org.springframework.context.ApplicationContext;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

@RestController
public class AdminController {
	Logger logger = LoggerFactory.getLogger(AdminController.class);

	@Autowired
	protected DataServer server;
	@Autowired
	protected DataRetriever retriever;

	@Autowired
	private ApplicationContext appContext;

	@RequestMapping(value = "/mht-server/admin/led/{value}")
	// 0: Permanent off
	// 1: Permanent on
	// 2: Auto based on time
	// curl -s localhost:8080/mht-server/admin/led/<int>
	public void led(@PathVariable("value") int value) {
		logger.info("Requested to change led to: " + value);
		try {
			server.setAdminLed(DataServer.LED.values()[value]);
		} catch (Exception e) {
		}		
	}

	@RequestMapping(value = "/mht-server/admin/poll/{value}")
	// 0: Permanent poll
	// 1: Permanent sleep
	// 2: Auto based on time
	// curl -s localhost:8080/mht-server/admin/poll/<int>
	public void awake(@PathVariable("value") int value) {
		logger.info("Requested to change awake to: " + value);
		try {
			retriever.setAdminPoll(DataRetriever.POLL.values()[value]);
		} catch (Exception e) {
		}
	}

	@RequestMapping(value = "/mht-server/admin/mode/{value}")
	// 0: Set mode to Division
	// 1: Set mode to Conference
	// 2: Set mode to League
	// 3: Set mode to Playoffs
	// 4: Set mode to Poweroff
	// 5: Set mode to None
	// 42: Cancel pending mode change
	// curl -s localhost:8080/mht-server/admin/mode/<int>
	public void mode(@PathVariable("value") int value) {
		logger.info("Requested to change mode to: " + value);
		server.setAdminMode(value);
	}

	@RequestMapping(value = "/mht-server/admin/unclean/{value}")
	// 0: Cancel pending unclean
	// 1: Mark as unclean once
	// curl -s localhost:8080/mht-server/admin/unclean/<int>
	public void unclean(@PathVariable("value") int value) {
		logger.info("Requested to mark unclean and powerdown: " + value);
		server.setAdminUnclean(value);
	}

	@RequestMapping(value = "/mht-server/admin/poweroff")
	// curl -s localhost:8080/mht-server/admin/poweroff
	public void poweroff() {
		logger.info("Admin requested poweroff.");
		SpringApplication.exit(appContext, new ExitCodeGenerator() {
			public int getExitCode() {
				return 0;
			}
		});
		System.exit(0);
	}
}
