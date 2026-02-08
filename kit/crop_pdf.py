#!/usr/bin/env python3
import fitz
import sys
import argparse


def parse_pages(pages_str, total_pages):
    """
    Parse page selection string.
    Supported: "all", "5", "1-3", "1,3,7"
    Converts 1-based input to 0-based valid indices.
    """
    if pages_str.lower() == "all":
        return list(range(total_pages))

    result = []
    parts = pages_str.split(",")

    for part in parts:
        part = part.strip()
        if not part:
            continue
        if "-" in part:
            try:
                start, end = map(int, part.split("-"))
            except ValueError:
                raise ValueError(f"Invalid page range: '{part}'")
            if start > end:
                raise ValueError(f"Invalid page range: start ({start}) > end ({end})")
            result.extend(range(start - 1, end))
        else:
            try:
                result.append(int(part) - 1)
            except ValueError:
                raise ValueError(f"Invalid page number: '{part}'")

    result = sorted(set(result))
    return [p for p in result if 0 <= p < total_pages]


def extract_region(page, w1, w2, h1, h2):
    """Convert fractional coordinates [0,1] to absolute PDF rect."""
    rect = page.rect
    x1 = rect.x0 + rect.width * w1
    x2 = rect.x0 + rect.width * w2
    y1 = rect.y0 + rect.height * h1
    y2 = rect.y0 + rect.height * h2
    return fitz.Rect(x1, y1, x2, y2)


def crop_pdf(
    input_pdf, output_pdf, *, mode=None, w1=None, w2=None, h1=None, h2=None, pages="all"
):
    """
    Crop PDF either by mode ('left', 'right', 'top', 'bottom')
    or by explicit fractional coordinates (w1, w2, h1, h2).
    Exactly one of {mode} or {w1,w2,h1,h2} must be provided.
    """
    if (mode is not None) == (w1 is not None):
        raise ValueError("Specify either --mode OR all of --w1, --w2, --h1, --h2.")

    doc = fitz.open(input_pdf)
    total_pages = len(doc)
    selected_pages = parse_pages(pages, total_pages)

    new_doc = fitz.open()
    new_doc.set_metadata(doc.metadata)  # Preserve author/title/etc.

    # page = doc[0]  # Just to access page dimensions
    # pix = page.get_pixmap()
    # pix.save("temp_preview.png")  # Save a preview image (optional)

    # Resolve cropping region
    if mode:
        mode = mode.lower()
        if mode == "left":
            w1, w2, h1, h2 = 0.0, 0.5, 0.0, 1.0
        elif mode == "right":
            w1, w2, h1, h2 = 0.5, 1.0, 0.0, 1.0
        elif mode == "top":
            w1, w2, h1, h2 = 0.0, 1.0, 0.5, 1.0
        elif mode == "bottom":
            w1, w2, h1, h2 = 0.0, 1.0, 0.0, 0.5
        else:
            raise ValueError(
                f"Unknown mode: '{mode}'. Use 'left', 'right', 'top', or 'bottom'."
            )
    else:
        # Validate fractional inputs
        if not (0 <= w1 < w2 <= 1):
            raise ValueError(
                f"Invalid width: w1={w1}, w2={w2}. Must satisfy 0 ≤ w1 < w2 ≤ 1."
            )
        if not (0 <= h1 < h2 <= 1):
            raise ValueError(
                f"Invalid height: h1={h1}, h2={h2}. Must satisfy 0 ≤ h1 < h2 ≤ 1."
            )

    for pno in selected_pages:
        page = doc[pno]
        print(f"page {pno} rotation =", page.rotation)
        crop_rect = extract_region(page, w1, w2, h1, h2)
        pix = page.get_pixmap(clip=crop_rect)
        pix.save(f"temp_page_{pno + 1}.png")  # Save cropped preview (optional)
        new_page = new_doc.new_page(width=crop_rect.width, height=crop_rect.height)
        new_page.show_pdf_page(
            new_page.rect, doc, pno, clip=crop_rect, rotate=page.rotation
        )
        # new_page.show_pdf_page(new_page.rect, doc, pno, clip=crop_rect)

    new_doc.save(output_pdf)
    new_doc.close()
    doc.close()


def main():
    parser = argparse.ArgumentParser(
        description="Crop PDF pages by direction or custom region.",
        formatter_class=argparse.RawTextHelpFormatter,
        epilog="""Examples:
  %(prog)s input.pdf output.pdf --mode left --pages "1-5"
  %(prog)s input.pdf output.pdf --w1 0 --w2 0.5 --h1 0.1 --h2 1 --pages "51"
  %(prog)s in.pdf out.pdf --mode right
""",
    )
    parser.add_argument("input", help="Input PDF file")
    parser.add_argument("output", help="Output PDF file")
    parser.add_argument(
        "--mode",
        choices=["left", "right", "top", "bottom"],
        help="Predefined crop direction",
    )
    parser.add_argument("--w1", type=float, help="Left boundary as fraction (0.0–1.0)")
    parser.add_argument("--w2", type=float, help="Right boundary as fraction (0.0–1.0)")
    parser.add_argument("--h1", type=float, help="Top boundary as fraction (0.0–1.0)")
    parser.add_argument(
        "--h2", type=float, help="Bottom boundary as fraction (0.0–1.0)"
    )
    parser.add_argument(
        "--pages",
        default="all",
        help='Pages to process (default: "all"). Examples: "1-3", "2,5", "1-10,15"',
    )

    args = parser.parse_args()

    # Validate mutual exclusivity
    if args.mode is not None:
        if any(v is not None for v in [args.w1, args.w2, args.h1, args.h2]):
            parser.error("Use --mode OR --w1/--w2/--h1/--h2, not both.")
        crop_pdf(args.input, args.output, mode=args.mode, pages=args.pages)
    elif all(v is not None for v in [args.w1, args.w2, args.h1, args.h2]):
        crop_pdf(
            args.input,
            args.output,
            w1=args.w1,
            w2=args.w2,
            h1=args.h1,
            h2=args.h2,
            pages=args.pages,
        )
    else:
        parser.error(
            "Either --mode must be specified, or all of --w1, --w2, --h1, and --h2."
        )

    print(f"Saved → {args.output}")


if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        print(f"Error: {e}", file=sys.stderr)
        sys.exit(1)
