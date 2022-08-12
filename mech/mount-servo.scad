$fn = 60;

//--------------------------------------------------------

	  w = 75;
	  b = 16;
	  difference() {
			cube([w, b, 6]);
			translate([w/2 - 20 / 2, b/2, 0]) cylinder(h=6, d=4.4);
			translate([w/2 - 20 / 2, b/2, 0]) cylinder(h=2, d=9);
			
			translate([w/2 + 20 / 2, b/2, 0]) cylinder(h=6, d=4.4);
			translate([w/2 + 20 / 2, b/2, 0]) cylinder(h=2, d=9);
	  }
	  
	  translate([w - 6, 0, 0]) cube([6, b, 6 + 50-11+29-1]);
	  translate([w - 6, b/2, 6]) rotate([90, 0, 0]) cylinder(h = b, d = 12, center = true);

translate([75, 0+16, 6 + 50+29-11-1]) rotate([-90, 0, 90]) difference() {
	  cube([41+16, 21+8, 6]);
	  translate([8, 8, 0]) cube([41, 21, 6]);
	  translate([4, 8+5, 0]) cylinder(h=6, d=4.4);
	  translate([4, 8++5+10, 0]) cylinder(h=6, d=4.4);
	  translate([41 + 8 + 4, 8+5, 0]) cylinder(h=6, d=4.4);
	  translate([41 + 8 + 4, 8+5+10, 0]) cylinder(h=6, d=4.4);
}