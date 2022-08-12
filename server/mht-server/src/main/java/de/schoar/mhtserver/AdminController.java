package de.schoar.mhtserver;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

@RestController
public class AdminController {
	Logger logger = LoggerFactory.getLogger(AdminController.class);

	@Autowired
	protected DataServer server;

	@RequestMapping(value = "/mht-server/admin/led/{value}")
	// curl -s localhost:8080/admin/led/<int>
	public void led(@PathVariable("value") int value) {
		logger.info("Requested to change led to: " + value);
		try {
			server.setAdminLed(DataServer.LED.values()[value]);
		} catch (Exception e) {
		}
	}

	@RequestMapping(value = "/mht-server/admin/mode/{value}")
	// curl -s localhost:8080/admin/mode/<int>
	public void mode(@PathVariable("value") int value) {
		logger.info("Requested to change mode to: " + value);
		server.setAdminMode(value);
	}

	@RequestMapping(value = "/mht-server/admin/unclean/{value}")
	// curl -s localhost:8080/admin/unclean/<int>
	public void unclean(@PathVariable("value") int value) {		
			logger.info("Requested to mark unclean and powerdown: " + value);
		server.setAdminUnclean(value);
	}

}
