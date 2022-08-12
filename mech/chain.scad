$fn = 60;

//--------------------------------------------------------

translate([0, 0, 0]) start();
translate([40, 0, 0]) middle();
translate([80, 0, 0]) end();

//--------------------------------------------------------

module start() {
	  difference() {
			cube([10, 30, 7]);
			translate([10/2, 10/2, 0]) cylinder(h=7, d=4.2);
			translate([10/2, 10/2, 4]) cylinder(h=3, d=7);

			translate([10/2, 30-10/2, 0]) cylinder(h=7, d=4.2);
			translate([10/2, 30-10/2, 4]) cylinder(h=7, d=7);
	  }


	  translate([0, 10 + 4, 0]) rotate([90, 0, 0]) difference() {
			cube([10+18, 18+7, 4]);
			translate([10 + 5, 18/2+7, 0]) cylinder(h=4, d=3.2);
			translate([10 + 5 + 8, 18/2+7, 0]) cylinder(h=4, d=3.2);
	  }
}

module middle() {
	  difference() {
			cube([16, 32, 7]);
			translate([0, 12.8, 10]) rotate([-30, 0, 0]) cube([30, 30, 30]);
			translate([16/2, 7, 0]) cylinder(h=7, d=4.2);
			translate([16/2, 7, 4]) cylinder(h=3, d=7);
	  }
	  
	  translate([0, 18 - 4, 7]) cube([16, 4, 20]);
	  translate([0, 18 - 4, 27]) rotate([-45, 0, 0]) cube([16, 4, 12]);
}

module end() {
	  w = 75;
	  difference() {
			cube([w, 14, 6]);
			translate([w/2 - 20 / 2, 14/2, 0]) cylinder(h=6, d=4.4);
			translate([w/2 - 20 / 2, 14/2, 0]) cylinder(h=2, d=9);
			
			translate([w/2 + 20 / 2, 14/2, 0]) cylinder(h=6, d=4.4);
			translate([w/2 + 20 / 2, 14/2, 0]) cylinder(h=2, d=9);

			translate([w/2 - 50 / 2, 14/2, 0]) cylinder(h=6, d=3.2);
			translate([w/2 - 50 / 2, 14/2, 0]) cylinder(h=2, d=7);
			translate([w/2 + 50 / 2, 14/2, 0]) cylinder(h=6, d=3.2);
			translate([w/2 + 50 / 2, 14/2, 0]) cylinder(h=2, d=7);
	  }

	  d = 36;
	  
	  translate([w, 0, -d]) cube([6, 14, d+6]);
	  
	  translate([w + 6, 14, -d]) rotate([90, 0, 0]) difference() {
			cube([18, 18, 6]);
			translate([5, 18/2, 0]) cylinder(h=6, d=3.2);
			translate([5, 18/2, 0]) cylinder(h=2, d=6);
			translate([5 + 8, 18/2, 0]) cylinder(h=6, d=3.2);
			translate([5 + 8, 18/2, 0]) cylinder(h=2, d=6);
	  }
}
