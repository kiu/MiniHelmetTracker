$fn = 60;

x = 68;
h = 14;
wbase = 4;
wwall = 2;

offset = 30;

//--------------------------------------------------------

base();
translate([(wwall + x + wwall) / 2, offset, h + 1]) insert(20);
translate([(wwall + x + wwall) / 2, offset, h + 40]) insert(-20);

//--------------------------------------------------------

module base() {
	  difference() {
		cube([wwall + x + wwall, wwall + x + wwall, wbase + h]);
			
		translate([wwall , wwall, -wbase]) cube([x, x, wbase + h]);

		translate([(wwall + x + wwall) / 2, offset, h + 1]) union() {
			  translate([0, 0, 0]) cube([38, 38, 2], center = true);
			  translate([0, 0, 2]) cube([33, 33, 2], center = true);
		}
		translate([(wwall + x + wwall) / 2, (wwall + x + wwall), h / 2]) rotate([90, 0, 0]) cylinder(h=wbase, d=4.8, center = true);
	}
}

module insert(angle) {
	  difference() {
			union() {
				  translate([0, 0, 2]) cube([32, 32, 2], center = true);
				  translate([0, 0, 0]) cube([36, 36, 2], center = true);
				  rotate([-15, 0, angle]) {
						translate([0, 0, 15 / 2 + 1]) cube([26, 2, 15], center = true);
						translate([0, -1, 15 + 2]) cube([26, 3 + 1, 2], center = true);
				  }
			}
			rotate([-15, 0, angle]) translate([0, -3, 1]) cube([26, 3, 4], center = true);
	 }
}
