import bpy, json, os

# 1) Adjust this to wherever you want the JSON file written:
out_path = os.path.expanduser("~/blender-work/curve-exports/curve.json")

obj = bpy.context.active_object
if obj is None or obj.type != 'CURVE':
    raise RuntimeError("Select a Curve object before running this script")

export_data = []

# 2) Iterate over each spline in the Curve data
for spline in obj.data.splines:
    if spline.type == 'BEZIER':
        pts = spline.bezier_points
        # Each bezier segment runs from pts[i] to pts[i+1]
        for i in range(len(pts)-1):
            p0 = pts[i].co
            h0 = pts[i].handle_right
            h1 = pts[i+1].handle_left
            p1 = pts[i+1].co
            export_data.append({
                "type": "bezier",
                "p0": [p0.x, p0.y, p0.z],
                "h0": [h0.x, h0.y, h0.z],
                "h1": [h1.x, h1.y, h1.z],
                "p1": [p1.x, p1.y, p1.z]
            })

    elif spline.type in {'POLY','NURBS'}:
        # fallback: export the raw control-points (no handles)
        pts = getattr(spline, 'points', spline.bezier_points)
        coords = [[p.co.x, p.co.y, p.co.z] for p in pts]
        export_data.append({
            "type": spline.type.lower(),
            "points": coords
        })

# 3) Write it out
with open(out_path, "w") as f:
    json.dump(export_data, f, indent=2)

print(f"Exported {len(export_data)} segments to {out_path!r}")
