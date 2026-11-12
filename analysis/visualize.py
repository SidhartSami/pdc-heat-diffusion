#!/usr/bin/env python3
"""Render heat-solver snapshots (.pvd / .pvti / .vti written by the C++ solver) to images.

No ParaView needed: this reads the solver's own VTK ImageData output (appended raw binary,
UInt32 header) and produces
  * a contact sheet of selected time steps (PNG),
  * an animation (GIF) of the full time series,
  * for 3D runs, the mid-plane z-slice is shown.

Usage:
  python analysis/visualize.py <snapshot_dir> [--out DIR] [--fps 8] [--cmap inferno]
The same files open directly in ParaView: File > Open > <snapshot_dir>/heat.pvd.
"""
import argparse
import re
import struct
import xml.etree.ElementTree as ET
from pathlib import Path

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt  # noqa: E402
import numpy as np  # noqa: E402
from matplotlib.animation import FuncAnimation, PillowWriter  # noqa: E402


def read_piece(path: Path):
    """Return (extent, cell array shaped (nz, ny, nx)) of one .vti piece."""
    raw = path.read_bytes()
    head_end = raw.index(b"<AppendedData")
    header = raw[:head_end].decode()
    m = re.search(r'<Piece Extent="([^"]+)"', header)
    x0, x1, y0, y1, z0, z1 = (int(v) for v in m.group(1).split())
    data_start = raw.index(b"_", head_end) + 1
    (nbytes,) = struct.unpack_from("<I", raw, data_start)
    vals = np.frombuffer(raw, dtype="<f8", count=nbytes // 8, offset=data_start + 4)
    nx, ny, nz = x1 - x0, y1 - y0, max(z1 - z0, 1)
    return (x0, y0, z0), vals.reshape(nz, ny, nx)


def read_snapshot(pvti: Path):
    root = ET.parse(pvti).getroot()
    whole = [int(v) for v in root.find("PImageData").attrib["WholeExtent"].split()]
    nx, ny, nz = whole[1], whole[3], max(whole[5], 1)
    field = np.zeros((nz, ny, nx))
    for piece in root.iter("Piece"):
        (x0, y0, z0), arr = read_piece(pvti.parent / piece.attrib["Source"])
        field[z0 : z0 + arr.shape[0], y0 : y0 + arr.shape[1], x0 : x0 + arr.shape[2]] = arr
    return field


def read_series(directory: Path):
    pvd = ET.parse(directory / "heat.pvd").getroot()
    return [(float(d.attrib["timestep"]), directory / d.attrib["file"]) for d in pvd.iter("DataSet")]


def mid_slice(field: np.ndarray) -> np.ndarray:
    return field[field.shape[0] // 2]


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("snapshots", type=Path)
    ap.add_argument("--out", type=Path, default=None)
    ap.add_argument("--fps", type=int, default=8)
    ap.add_argument("--cmap", default="inferno")
    args = ap.parse_args()
    out = args.out or args.snapshots
    out.mkdir(parents=True, exist_ok=True)

    series = read_series(args.snapshots)
    fields = [(t, mid_slice(read_snapshot(p))) for t, p in series]
    vmin = min(f.min() for _, f in fields)
    vmax = max(f.max() for _, f in fields)
    print(f"{len(fields)} snapshots, field range [{vmin:.4g}, {vmax:.4g}]")

    # Contact sheet: up to 6 evenly spaced frames.
    idx = np.unique(np.linspace(0, len(fields) - 1, min(6, len(fields))).astype(int))
    fig, axes = plt.subplots(1, len(idx), figsize=(3.2 * len(idx), 3.5), squeeze=False)
    for ax, i in zip(axes[0], idx):
        t, f = fields[i]
        im = ax.imshow(f, origin="lower", extent=(0, 1, 0, 1), vmin=vmin, vmax=vmax, cmap=args.cmap)
        ax.set_title(f"t = {t:.4g}", fontsize=10)
        ax.set_xlabel("x")
    axes[0][0].set_ylabel("y")
    fig.colorbar(im, ax=axes[0].tolist(), shrink=0.85, label="temperature")
    fig.savefig(out / "heat_frames.png", dpi=140, bbox_inches="tight")
    plt.close(fig)

    # Animation
    fig, ax = plt.subplots(figsize=(4.6, 4.2))
    im = ax.imshow(fields[0][1], origin="lower", extent=(0, 1, 0, 1), vmin=vmin, vmax=vmax, cmap=args.cmap)
    ttl = ax.set_title(f"t = {fields[0][0]:.4g}")
    ax.set_xlabel("x")
    ax.set_ylabel("y")
    fig.colorbar(im, ax=ax, label="temperature")

    def update(k):
        im.set_data(fields[k][1])
        ttl.set_text(f"t = {fields[k][0]:.4g}")
        return im, ttl

    anim = FuncAnimation(fig, update, frames=len(fields), blit=False)
    anim.save(out / "heat_animation.gif", writer=PillowWriter(fps=args.fps))
    plt.close(fig)
    print(f"wrote {out / 'heat_frames.png'} and {out / 'heat_animation.gif'}")


if __name__ == "__main__":
    main()
