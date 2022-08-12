$fn = 60;

x = 39;
y = 99;
h = 17 + 8;

w = 4;

//--------------------------------------------------------

difference() {
	  union() {
			difference() {
				  cube([x + 2 * w, y + 2 * w, h + w]);
				  translate([w , w, w]) cube([x, y, h]);
			}
			translate([w , w, w]) cube([13, y, 8]);
	  }
	  translate([0 , w + y - 5.5, w + 8 + 5]) rotate([0, 90, 0]) cylinder(h=x + 2 * w, d=4);
	  translate([w + x , (w + y + w) / 4 * 1, w + h - 8]) rotate([0, 90, 0]) cylinder(h=w, d=8);
	  translate([w + x , (w + y + w) / 4 * 3, w + h - 8]) rotate([0, 90, 0]) cylinder(h=w, d=8);
}

difference() {
	  translate([0 , w + y, 0]) cube([w + x + w, 8, (126 + w +8) / 2 + 30]);
	  translate([(w + x + w) / 2 , w + y, w + 8 + (126 / 2) + 10]) rotate([-90, 0, 0]) cylinder(h=8, d=4.6);
	  translate([(w + x + w) / 2 , w + y, w + 8 + (126 / 2) - 10]) rotate([-90, 0, 0]) cylinder(h=8, d=4.6);

	  translate([(w + x + w) / 2 , w + y - 4, w + 8 + (126 / 2) + 10]) rotate([-90, 0, 0]) cylinder(h=8, d=8);
	  translate([(w + x + w) / 2 , w + y - 4, w + 8 +(126 / 2) - 10]) rotate([-90, 0, 0]) cylinder(h=8, d=8);

}