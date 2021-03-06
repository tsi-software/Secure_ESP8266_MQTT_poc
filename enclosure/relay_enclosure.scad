// relay_enclosure.scad
// Copyright (c) 2021 Warren Taylor

use </home/wtaylor/data/openscad/MCAD/boxes.scad>

cli_fn = 32; // This value can be changed by the command line.
$fn = cli_fn;
fn_Low  = 32;
fn_Med  = fn_Low * 2;
fn_High = fn_Low * 4;

//$fn = $preview ? 12 : 72;

inchesToMm = 25.4;
wallWidth = 2.0;
supportWidth = 3.25;
spacing = 1 + (4.2 - 1.55);
gap = 0.5;
small_gap = 0.12;
omega = 0.001;

base_corner_radius = 1;


relay_board_width = 1.72;
controller_board_width = 4.0;
full_relay_board_width = 1.55;
full_controller_board_width = 4.0;

relay_board =     [138.1+gap*2, 50.26+gap*2, 1.72+gap*1];
controller_board = [88.0+gap*2, 56.40+gap*2, 1.55+gap*1];
board_separation = 47.0;

lower_enclosure_inside = [
    max(relay_board.x, controller_board.x) + gap*0,
    board_separation + relay_board.z + controller_board.z + spacing*3,
    max(relay_board.y, controller_board.y),
];
lower_enclosure_outside = [
    lower_enclosure_inside.x + wallWidth*2,
    lower_enclosure_inside.y + wallWidth*2,
    lower_enclosure_inside.z + wallWidth*1,
];

support_rail = [
    supportWidth,
    spacing * 2 + supportWidth,
    min(relay_board.y, controller_board.y),
];


lock_screw_hole_diameter = 5.5;
lock_screw_hole_radius = lock_screw_hole_diameter/2;
lock_screw_hole_offset = 10;

upper_enclosure_inside = [
    lower_enclosure_outside.x + gap*2,
    lower_enclosure_outside.y + gap*2,
    lower_enclosure_outside.z + gap*2 + lock_screw_hole_offset + lock_screw_hole_diameter + supportWidth
];
upper_enclosure_outside = [
    upper_enclosure_inside.x + wallWidth*2,
    upper_enclosure_inside.y + wallWidth*2,
    upper_enclosure_inside.z + wallWidth*1,
];
hangerWidth = supportWidth;


translate([0, -lower_enclosure_outside.y*0.6, 0]) {
    lower_enclosure();
}

translate([0, upper_enclosure_outside.y*0.6, 0]) {
    upper_enclosure();
}



module circuit_boards() {
    translate([0, (relay_board.z + board_separation)/2, relay_board.y/2])
    rotate([90,0,0])
    {
        cube(relay_board, center = true);
    }

    translate([0, -(controller_board.z + board_separation)/2, controller_board.y/2])
    rotate([90,0,0])
    {
        cube(controller_board, center = true);
    }
}


module lower_enclosure() {
    difference() {
        union() {
            // Outside & Inside walls of lower enclosure.
            translate([0, 0, lower_enclosure_inside.z/2]) {
                difference() {
                    translate([0, 0, -(wallWidth/2 + omega)]) {
                        roundedCube(
                            size = lower_enclosure_outside,
                            r = base_corner_radius,
                            sidesonly = false,
                            center = true,
                            $fn = fn_High
                        );
                    }
                    translate([0, 0, base_corner_radius]) {
                        roundedCube(
                            size = [
                                lower_enclosure_inside.x,
                                lower_enclosure_inside.y,
                                lower_enclosure_inside.z + base_corner_radius*2
                            ],
                            r = base_corner_radius,
                            sidesonly = false,
                            center = true,
                            $fn = fn_Low
                        );
                    }//translate
                }//difference
            }//translate

            // Support Rails for relay board.
            for (offset = [-1, +1]) {
                translate([
                    offset * (lower_enclosure_inside.x/2 - support_rail.x/2 + omega),
                    lower_enclosure_inside.y/2 - support_rail.y/2 + omega,
                    support_rail.z/2 - omega
                ])
                {
                    cube(support_rail, center=true);
                }//translate
            }//for

            // Support Rails for controller board.
            for (offset = [-1, +1]) {
                translate([
                    offset * (controller_board.x*0.5 - support_rail.x*0 + omega),
                    -(lower_enclosure_inside.y/2 - support_rail.y/2 + omega),
                    support_rail.z/2 - omega
                ])
                {
                    cube([support_rail.x*2, support_rail.y, support_rail.z], center=true);
                }//translate
            }//for
        }//union

        //++ difference ++
        #circuit_boards();
    }
}


module upper_enclosure() {
    // Outside & Inside walls of upper enclosure.
    translate([0, 0, upper_enclosure_inside.z/2])
    //rotate([0,180,0]) //JUST TESTING!!!
    {
        difference() {
            translate([0, 0, -(wallWidth/2 + omega)]) {
                union() {
                    // outside
                    roundedCube(
                        size = upper_enclosure_outside,
                        r = base_corner_radius + gap,
                        sidesonly = false,
                        center = true,
                        $fn = fn_High
                    );
                    translate([0, -upper_enclosure_outside.y*0.5, -upper_enclosure_inside.z/2]) {
                        hanger();
                    }
                }
            }
            //++ difference ++
            translate([0, 0, base_corner_radius]) {
                // inside
                roundedCube(
                    size = [
                        upper_enclosure_inside.x,
                        upper_enclosure_inside.y,
                        upper_enclosure_inside.z + base_corner_radius*2
                    ],
                    r = base_corner_radius + gap,
                    sidesonly = false,
                    center = true,
                    $fn = fn_Low
                );

                // Lock Screw holes.
                translate([0, 0, upper_enclosure_inside.z/2 - lock_screw_hole_diameter - supportWidth])
                rotate([0,90,0])
                {
                    cylinder(
                        r = lock_screw_hole_radius,
                        h = upper_enclosure_outside.x*1.5,
                        center = true,
                        $fn = fn_High
                    );
                    //lock_screw_hole_radius
                    //lock_screw_hole_offset
                }

            }//translate
        }//difference
    }//translate
}


module hanger() {
    screw_cap_diameter = 6;
    screw_cap_radius = screw_cap_diameter/2;
    screw_thread_diameter = 4;
    screw_thread_radius = screw_thread_diameter/2;

    hanger_offset_r1 = screw_cap_radius + supportWidth; // + hangerWidth; //+hangerWidth to create a bevel.
    hanger_offset_r2 = screw_cap_radius + supportWidth;

    hanger_centers = [
        [
            upper_enclosure_outside.x*0.5,
            0,
            upper_enclosure_outside.z*0.66
        ],
        [
            upper_enclosure_outside.x + hanger_offset_r1 + wallWidth,
            0,
            hanger_offset_r1+base_corner_radius
        ],
        [
            0 - (hanger_offset_r1 + wallWidth),
            0,
            hanger_offset_r1+base_corner_radius
        ],
    ];

    translate([-hanger_centers[0].x, hangerWidth/2, 0])
    difference()
    {
        hull() {
            for (ndx = [0 : len(hanger_centers)-1]) {
                translate(hanger_centers[ndx]) {
                    if (ndx == -1) {
                        //
                    } else {
                        rotate([-90, 0, 0]) {
                            //TODO: use a donut here instead of a cylinder.
                            cylinder(
                                r1 = hanger_offset_r1,
                                r2 = hanger_offset_r2,
                                h = hangerWidth,
                                center = true,
                                $fn = fn_Low
                            );
                        }
                    }//if-else
                }//translate
            }//for
        }//hull
        //++ difference ++
        for (ndx = [0 : len(hanger_centers)-1]) {
            translate(hanger_centers[ndx]) {
                if (ndx == 0) {
                    //
                } else {
                    rotate([-90, 0, 0]) {
                        cylinder(
                            r = screw_thread_radius,
                            h = hangerWidth+1,
                            center = true,
                            $fn = fn_Low
                        );
                    }
                }//if-else
            }//translate
        }//for
    }//difference & translate
}
