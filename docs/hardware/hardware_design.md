# Hardware Design Guide

## 1. Mechanical

Document frame material, fasteners, galvanic compatibility, buoyancy, ballast, pressure enclosures, O-rings, penetrators, seals, thruster guards, tether strain relief, lift points, and recovery attachment. Maintain CAD coordinate systems aligned or explicitly transformed to BODY.

## 2. Buoyancy and ballast

Target slight positive buoyancy unless mission needs dictate otherwise. Design ballast to keep center of gravity below center of buoyancy and preserve trim with payload changes. Record mass and displaced-volume budget by component.

## 3. Pressure boundary

Pressure test unmanned using an appropriate controlled method and safety barrier. Inspect sealing surfaces, O-ring material and lubrication compatibility, groove geometry, penetrator torque, and cable strain. A pressure or vacuum test does not eliminate the need for leak detection.

## 4. Electrical power budget

For each load, record nominal voltage, average current, peak current, startup surge, duty cycle, fuse, wire gauge, connector, and thermal assumptions. Approximate battery endurance is

$$
t_{ideal}=\frac{C_{Ah}}{I_{avg}},
$$

but apply derating for usable capacity, temperature, voltage sag, aging, converter losses, and reserve. Energy-based budgeting is

$$
E_{load}=\int V(t)I(t)dt.
$$

Fuses primarily protect wiring and should be selected with conductor ampacity, fault current, and transient behavior in mind.

## 5. Grounding and EMC

Separate high-current motor paths from sensitive sensor wiring, use intentional return paths, minimize loop area, route differential communications as specified, and place suppression near noise sources. Verify magnetometer location under realistic thruster current.

## 6. Connectors and tether

Document wet-mate versus dry-mate restrictions, pinout, keying, sealing, voltage/current rating, bend radius, pull strength, and strain relief. The tether must not transfer uncontrolled load to pressure penetrators.

## 7. Sensors

Record part, range, resolution, update rate, latency, interface, axes, mounting transform, environment rating, calibration, and failure behavior. Locate pressure sensing to minimize trapped air and dynamic pressure bias.

## 8. Hardware release checklist

- Schematics reviewed
- Polarity and pinout independently checked
- Fusing and isolation verified
- Continuity/insulation tests completed
- Enclosure and penetrator inspection complete
- Pressure test evidence stored
- Mass, buoyancy, CG, and trim measured
- Guards and emergency stop available
- Firmware and calibration revisions recorded
