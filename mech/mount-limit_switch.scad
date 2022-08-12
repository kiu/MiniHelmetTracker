$fn = 60;
thick = 3.0;

dia_rail = 4.3;
dia_switch = 2.3;

// --------------------------------------------

difference() {
	cube([16, 20, thick]);
	translate([12, 10, 0]) {
		cylinder(h = thick, d = dia_rail);
	}
}

translate([0, 10 + thick, 0]) {
	rotate([90, 0, 0]) {
		difference()  {
			cube([7, 16 + thick, thick]);	
			translate([0, thick + 6, 0]) {
				translate([4, 0, 0]) {
					cylinder(h = thick, d = dia_switch);
				}
				translate([4, 6.5, 0]) {
					cylinder(h = thick, d = dia_switch);
				}
			}
		}
	}
}
