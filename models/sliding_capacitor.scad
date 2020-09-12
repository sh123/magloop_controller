/* [Main Parameters] */

// count of all plates in capacitor
plate_count_ = 13;           // [3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25]

// distance between rotor and stator capacitor plates in mm
plate_distance_ = 5;         // [2, 3, 4, 5, 6, 7, 8]

// capacitor plate width
plate_width_ = 55;

// capacitor plate height
plate_depth_ = 75;

// capacitor plate hole height, use larger value to make some room
plate_height_ = 0.6;

// gap between rotor and stator, tune for smoother sliding by printing tiny capacitor first, use acetone bath!
sliding_gap_ = 0.1;

// capacitor element to generate
generate_element_ = "stator";       // [stator, rotor]

// generate both stator and rotor for demo purposes?
generate_both_demo_ = "yes";        // [yes, no]

// enable worm gear system?
enable_worm_gear_ = "yes";          // [yes, no]

// enable button pusher?
enable_rotor_button_pusher_ = "yes";    // [yes, no]

/* [Worm Gear Parameters] */

// how much space to provide for worm gear system
worm_gear_gap_ = 6;

// worm shaft radius, change if use other than M4 shaft
worm_shaft_radius_ = 2;     // [1, 1.5, 2, 2.5, 3]

// bearing outer radius
bearing_radius_ = 4;

// bearing size depth/height
bearing_depth_ = 3;

// hole for screws, use larger diameter so tool will fit
screw_hole_radius_ = 6;

// depth of hole for screw, change based on shaft, number of nuts
screw_hole_depth_ = 8;

// hole sizes for stepper holder screws
holder_screw_radius_ = 1.5;

// distance from the shaft to the the motor holding screws
holder_screw_center_distance_ = 16;

// height of rotor button pusher    
button_pusher_height_ = 7;

// hole depth for holding nut on the rotor, change based on shaft, M3/M4/etc
nut_cutout_depth_ = 3.2;

// holding nut radius on the rotor, change based on shaft, M3/M4/etc
nut_cutout_radius_ = 4;
    
/* [Advanced Parameters] */

// gap between plate edge and stator edge
plate_width_guard_ = 1.5;

// depth of rotor base plate
rotor_base_plate_depth_ = 5;

assert(plate_count_ % 2 == 1, "Plate count must be an odd number");

echo("maximum capacitance: ~", capacitor_value_pf(plate_width_, plate_depth_, plate_count_, plate_distance_), "pF");
echo("breakdown voltage for air: ", breakdown_voltage_kv(3, plate_distance_), "kV");
echo("breakdown voltage for PETG: ", breakdown_voltage_kv(4.5, plate_distance_), "kV");

if (enable_worm_gear_ == "yes")
{
    assert(plate_count_ % 4 == 1, "Wrong plate count, must be one of [5, 9, 13, 17, 21, 25]");
    capacitor_with_worm_gear(plate_count_, plate_distance_, plate_width_, 
        plate_depth_, plate_height_, sliding_gap_, generate_element_, 
        generate_both_demo_, worm_gear_gap_, enable_rotor_button_pusher_);
} else 
{
    capacitor_naked(plate_count_, plate_distance_, plate_width_, 
        plate_depth_, plate_height_, sliding_gap_, generate_element_, generate_both_demo_);
}

// calculate capacitor breakdown voltage
function breakdown_voltage_kv(breakdown_voltage_mm, plate_distance) = plate_distance * breakdown_voltage_mm;

// calculate ~20% infill PETG filament approximate capacitor value from dimensions in mm and plate count
function capacitor_value_pf(plate_width, plate_depth, plate_count, plate_distance) =
    let (dielectric_constant = 1.55, electrostatic_constant = 8.854e-12)
        (plate_count - 1) * dielectric_constant * electrostatic_constant * plate_width * plate_depth / plate_distance * 1e9;

// generate capacitor with worm gear
module capacitor_with_worm_gear(plate_count, plate_distance, plate_width, 
    plate_depth, plate_height, sliding_gap, generate_element, generate_both_demo, 
    worm_gear_gap, enable_rotor_button_pusher)
{
    if (generate_element == "stator" || generate_both_demo == "yes") 
    {
        translate([0, 75, 0])
            capacitor_stator_with_worm_gear(plate_count, plate_distance, 
                plate_width, plate_depth, plate_height, worm_gear_gap);
    }
    if (generate_element == "rotor" || generate_both_demo == "yes") 
    {
        color("green")
            capacitor_rotor_with_worm_gear(plate_count, plate_distance, 
                plate_width, plate_depth, plate_height, sliding_gap, worm_gear_gap, enable_rotor_button_pusher);
    }
}

// generate capacitor
module capacitor_naked(plate_count, plate_distance, plate_width, 
    plate_depth, plate_height, sliding_gap, generate_element, generate_both_demo)
{
    if (generate_element == "stator" || generate_both_demo == "yes") 
    {
        translate([0, 50, 0])
            capacitor_stator(plate_count, plate_distance, 
                plate_width, plate_depth, plate_height);
    }
    if (generate_element == "rotor" || generate_both_demo == "yes") 
    {
        color("green")
            capacitor_rotor(plate_count, plate_distance, 
                plate_width, plate_depth, plate_height, sliding_gap);
    }
}

// generate stator with worm gear support
module capacitor_stator_with_worm_gear(plate_count, plate_distance, 
    plate_width, plate_depth, plate_height, worm_gear_gap)
{
    plate_width_guard = plate_width_guard_;
    worm_shaft_radius = worm_shaft_radius_;
    bearing_radius = bearing_radius_;
    bearing_depth = bearing_depth_;
    screw_hole_radius = screw_hole_radius_;
    screw_hole_depth = screw_hole_depth_;
    holder_screw_radius = holder_screw_radius_;
    holder_screw_center_distance = holder_screw_center_distance_;
    
    holder_screw_depth = plate_depth;
    bearing_full_depth = bearing_depth + screw_hole_depth;
    half_plate_count = ceil(plate_count / 2.0);
    box_height = half_plate_count * (plate_distance + plate_height);
    box_height_center = box_height + worm_gear_gap / 2.0;
    box_width_center = plate_width / 2.0 + plate_width_guard;
    
    difference()
    {
        // stator body with holes
        union()
        {
            // two split capacitor stators
            translate([0, 0, 0])
                capacitor_stator(half_plate_count, plate_distance, plate_width, plate_depth, plate_height);
            translate([0, 0, box_height + worm_gear_gap])
                capacitor_stator(half_plate_count, plate_distance, plate_width, plate_depth, plate_height);
            
            // worm gear blade hole
            translate([0, 0, box_height])
                cube([plate_width + 2 * plate_width_guard, plate_depth, worm_gear_gap]);
        }
        
        // worm gear system
        translate([plate_width / 2.0 + plate_width_guard, 0, box_height_center])
            rotate([-90, 0, 0])
                union() 
                {
                    cylinder(plate_depth, r=worm_shaft_radius);
                    cylinder(bearing_full_depth, r=bearing_radius);
                    cylinder(screw_hole_depth, r=bearing_radius + 1);
                }
        translate([box_width_center, plate_depth - bearing_full_depth, box_height_center])
            rotate([-90, 0, 0])
                cylinder(bearing_full_depth, r=bearing_radius);
        translate([box_width_center, plate_depth - screw_hole_depth, box_height_center])
            rotate([-90, 0, 0])
                cylinder(screw_hole_depth, r=screw_hole_radius);

        // holder holes
        translate([box_width_center + holder_screw_center_distance, plate_depth - holder_screw_depth, box_height_center])
            rotate([-90, 0, 0])
                cylinder(holder_screw_depth, r=holder_screw_radius);
        translate([box_width_center - holder_screw_center_distance, plate_depth - holder_screw_depth, box_height_center])
            rotate([-90, 0, 0])
                cylinder(holder_screw_depth, r=holder_screw_radius);
    }
}

// generate rotor with worm gear support
module capacitor_rotor_with_worm_gear(plate_count, plate_distance, 
    plate_width, plate_depth, plate_height, sliding_gap, 
    worm_gear_gap, enable_rotor_button_pusher)
{
    plate_width_guard = plate_width_guard_;
    worm_shaft_radius = worm_shaft_radius_;
    base_plate_width = rotor_base_plate_depth_;
    nut_cutout_depth = nut_cutout_depth_;
    nut_cutout_radius = nut_cutout_radius_;
    button_pusher_height = button_pusher_height_;

    half_plate_count = ceil(plate_count / 2.0);
    
    box_height = half_plate_count * (plate_distance + plate_height);
    box_height_center = box_height + worm_gear_gap / 2.0;
    box_width_center = plate_width / 2.0 + plate_width_guard;
    
    button_pusher_height_pos = 2 * box_height + worm_gear_gap;
    button_pusher_width = plate_width / 4.0;
    button_pusher_width_center = box_width_center - button_pusher_width / 2.0;
    button_pusher_depth = worm_gear_gap;

    difference()
    {
        // rotor body with blades
        union()
        {
            // two split capacitor rotors
            translate([0, 0, 0])
                capacitor_rotor(half_plate_count, plate_distance, plate_width, 
                    plate_depth, plate_height, sliding_gap);
            translate([0, 0, box_height + worm_gear_gap])
                capacitor_rotor(half_plate_count, plate_distance, plate_width, 
                    plate_depth, plate_height, sliding_gap);
            
            // worm gear blade
            translate([0, 0, box_height])
                cube([plate_width + 2 * plate_width_guard, base_plate_width, worm_gear_gap]);

            if (enable_rotor_button_pusher == "yes") 
            {
                translate([button_pusher_width_center, 0, button_pusher_height_pos])
                    cube([button_pusher_width, button_pusher_depth, button_pusher_height]);
            }
        }

        // worm gear hole
        translate([box_width_center, 0, box_height_center])
            rotate([-90, 0, 0])
                cylinder(base_plate_width, r=worm_shaft_radius);

        // nut cutout
        translate([box_width_center, 0, box_height_center])
            rotate([-90, 0, 0])
                cylinder(r=nut_cutout_radius, h=nut_cutout_depth, $fn=6);
    }
}

// generate sliding capacitor stator
module capacitor_stator(plate_count, plate_distance, plate_width, plate_depth, plate_height) 
{
    plate_width_guard = plate_width_guard_;

    // calculated dimensions
    box_width = plate_width + (plate_width_guard * 2);
    box_depth = plate_depth;
    box_height = plate_count * plate_distance + plate_count * plate_height;

    echo("stator dimensions:", box_width, box_depth, box_height);

    stator_plate_count = ceil(plate_count / 2.0);
    stator_plate_distance = 2 * (plate_distance + plate_height);
    stator_plate_initial_height = plate_distance / 2.0;

    echo("stator plates:", stator_plate_distance, stator_plate_initial_height);

    stator_hole_count = floor(plate_count / 2.0);
    stator_hole_distance = stator_plate_distance;
    stator_hole_initial_height = plate_distance + plate_height;
    stator_hole_height = plate_distance + plate_height;

    echo("stator holes:", stator_hole_distance, stator_hole_initial_height);

    difference() 
    {
        // stator body
        cube([box_width, box_depth, box_height]);

        // holes for plates
        for (plate_num = [0:stator_plate_count - 1]) 
        {
            translate([plate_width_guard, 0, plate_num * stator_plate_distance + stator_plate_initial_height])
                cube([plate_width, plate_depth, plate_height]);
        }
        
        // holes for rotor blades
        for (plate_num = [0:stator_hole_count - 1]) 
        {
            translate([plate_width_guard, 0, plate_num * stator_hole_distance + stator_hole_initial_height])
                cube([plate_width, plate_depth, stator_hole_height]);
        }
    }
}

// generate sliding capacitor rotor
module capacitor_rotor(plate_count, plate_distance, plate_width, plate_depth, plate_height, sliding_gap) 
{
    plate_width_guard = plate_width_guard_;

    // calculated dimensions
    box_width = plate_width + (plate_width_guard * 2);
    box_depth = rotor_base_plate_depth_;
    box_height = plate_count * plate_distance + plate_count * plate_height;

    echo("rotor dimensions:", box_width, box_depth, box_height);

    rotor_plate_count = ceil(plate_count / 2.0);
    rotor_plate_distance = 2 * (plate_distance + plate_height);
    rotor_plate_initial_height = plate_distance + plate_height + plate_distance / 2.0;

    echo("rotor plates:", rotor_plate_distance, rotor_plate_initial_height);

    rotor_blade_count = floor(plate_count / 2.0);
    rotor_blade_distance = rotor_plate_distance;
    rotor_blade_initial_height = plate_distance + plate_height + sliding_gap;
    rotor_blade_height = plate_distance + plate_height - 2 * sliding_gap;

    echo("rotor blades:", rotor_blade_distance, rotor_blade_initial_height);

    difference() 
    {
        // rotor body
        union() 
        {
            // rotor base plate
            cube([box_width, box_depth, box_height]);
            
            // rotor blades
            for (plate_num = [0:rotor_blade_count - 1]) 
            {
                translate([plate_width_guard + sliding_gap, 0, plate_num * rotor_blade_distance + rotor_blade_initial_height])
                    cube([plate_width - 2 * sliding_gap, plate_depth + box_depth - sliding_gap, rotor_blade_height]);
            }
        }
        
        // rotor plates 
        for (plate_num = [0:rotor_plate_count - 1]) 
        {
            translate([2 * plate_width_guard, 0, plate_num * rotor_plate_distance + rotor_plate_initial_height])
                cube([plate_width - 2 * plate_width_guard, plate_depth + box_depth, plate_height]);
        }
    }
}