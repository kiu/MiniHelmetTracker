$fn = 60;

w = 20;
t = 8;

//--------------------------------------------------------

translate([30, 0, 0]) {
difference() {
	  union() {
			cube([w, 60, t]);
			translate([0, t, 0]) rotate([0, 90, 0]) cylinder(h=w, d= t*2);
	  }
	  translate([0, 12 - 4, t - 3]) rotate([50, 0, 0]) cube([w, w, w]);
}
translate([0, 17.22 - 4, t - 3]) rotate([50, 0, 0]) cube([w, 8, 4]);

translate([0,0, -35]) difference() {
	  cube([w, t, 35]);
	  translate([w/2, 0, 10]) {
			rotate([-90, 0, 0]) cylinder(h=t, d= 4.4);
			translate([-4.4/2, 0, 0]) cube([4.4, t, 6]);
			translate([0, 0, 6]) rotate([-90, 0, 0]) cylinder(h=t, d= 4.4);
	  }
	  translate([w/2, t-2, 10]) {
			rotate([-90, 0, 0]) cylinder(h=t, d= 7.2);
			translate([-7.2/2, 0, 0]) cube([7, t, 6]);
			translate([0, 0, 6]) rotate([-90, 0, 0]) cylinder(h=t, d= 7.2);
	  }
}

translate([0, 30, t-2]) rotate([15, 0, 0]) cube([w, 26, 2]);
}