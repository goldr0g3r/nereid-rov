# Parameter Dictionary

Every parameter record should include:

```text
name
symbol
description
value
SI unit
shape
coordinate frame
sign convention
valid range
uncertainty
source type: measured / identified / supplier / calculated / assumed
source artifact
calibration or experiment ID
revision
author
review status
```

Core groups include mass and inertia; centers of gravity and buoyancy; displaced volume and density; added mass; linear and quadratic damping; thruster position/direction/map/dynamics; sensor transforms/noise/bias; tether; environment; controller gains and limits; timing; safety thresholds.

Never store the same authoritative physical value independently in Python and Simulink. Generate target-specific representations from one reviewed source.
