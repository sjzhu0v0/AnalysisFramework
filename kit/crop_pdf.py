#!/usr/bin/env python3
import argparse
import sys
import fitz  # PyMuPDF


def parse_pages(pages_str: str, total_pages: int):
    """Supported: all, 5, 1-3, 1,3,7 (1-based input -> 0-based indices)."""
    if pages_str.lower() == "all":
        return list(range(total_pages))

    result = []
    for part in pages_str.split(","):
        part = part.strip()
        if not part:
            continue
        if "-" in part:
            s, e = part.split("-", 1)
            start, end = int(s), int(e)
            if start > end:
                raise ValueError(f"Invalid range: {part}")
            result.extend(range(start - 1, end))
        else:
            result.append(int(part) - 1)

    result = sorted(set(result))
    bad = [p + 1 for p in result if p < 0 or p >= total_pages]
    if bad:
        raise ValueError(f"Page(s) out of range: {bad}")
    return result


def frac_to_rect(page: fitz.Page, w1: float, w2: float, h1: float, h2: float) -> fitz.Rect:
    """Fractional [0,1] coords on the *visual* page -> Rect in that visual coord system."""
    r = page.rect  # visual rect (already accounts for rotation)
    return fitz.Rect(
        r.x0 + r.width * w1,
        r.y0 + r.height * h1,
        r.x0 + r.width * w2,
        r.y0 + r.height * h2,
    )


def resolve_mode(mode: str):
    mode = mode.lower()
    if mode == "left":
        return 0.0, 0.5, 0.0, 1.0
    if mode == "right":
        return 0.5, 1.0, 0.0, 1.0
    if mode == "top":
        return 0.0, 1.0, 0.0, 0.5
    if mode == "bottom":
        return 0.0, 1.0, 0.5, 1.0
    if mode == "full":
        return 0.0, 1.0, 0.0, 1.0
    raise ValueError(f"Unknown mode: {mode}")


def crop_pdf(input_pdf: str, output_pdf: str, *, mode=None, w1=None, w2=None, h1=None, h2=None, pages="all"):
    # validate
    custom = all(v is not None for v in (w1, w2, h1, h2))
    if (mode is not None) == custom:
        raise ValueError("Specify either --mode OR all of --w1, --w2, --h1, --h2.")

    doc = fitz.open(input_pdf)
    out = fitz.open()
    out.set_metadata(doc.metadata)

    selected = parse_pages(pages, doc.page_count)

    # decide crop fractions
    if mode:
        w1, w2, h1, h2 = resolve_mode(mode)
    else:
        if not (0 <= w1 < w2 <= 1 and 0 <= h1 < h2 <= 1):
            raise ValueError("Fractions must satisfy 0 ≤ w1 < w2 ≤ 1 and 0 ≤ h1 < h2 ≤ 1")

    for pno in selected:
        page = doc[pno]
        rot = page.rotation  # 0/90/180/270

        # 1) crop rect in *visual* coords (what you see after rotation)
        # page.set_rotation(0)
        crop_view = frac_to_rect(page, float(w1), float(w2), float(h1), float(h2))

        # 2) map to *source* coords for show_pdf_page clip (core fix)
        crop_src = crop_view * page.derotation_matrix
        page.set_rotation(0)

        new_page = out.new_page(width=crop_src.width, height=crop_src.height)

        new_page.show_pdf_page(new_page.rect, doc, pno, clip=crop_src)
        new_page.set_rotation(rot)

    out.save(output_pdf)
    out.close()
    doc.close()


def main():
    ap = argparse.ArgumentParser(description="Crop PDF by visual fractions (rotation-safe).")
    ap.add_argument("input", help="Input PDF")
    ap.add_argument("output", help="Output PDF")
    ap.add_argument("--mode", choices=["left", "right", "top", "bottom", "full"], help="Predefined crop region")
    ap.add_argument("--w1", type=float, help="Left (0-1)")
    ap.add_argument("--w2", type=float, help="Right (0-1)")
    ap.add_argument("--h1", type=float, help="Top (0-1)")
    ap.add_argument("--h2", type=float, help="Bottom (0-1)")
    ap.add_argument("--pages", default="all", help='Pages (e.g. "1-3", "2,5", "all")')
    args = ap.parse_args()

    crop_pdf(
        args.input,
        args.output,
        mode=args.mode,
        w1=args.w1,
        w2=args.w2,
        h1=args.h1,
        h2=args.h2,
        pages=args.pages,
    )
    print(f"Saved → {args.output}")


if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        print(f"Error: {e}", file=sys.stderr)
        sys.exit(1)
