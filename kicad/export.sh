export_svg () {
    kicad-cli pcb export svg tp.kicad_pcb \
    --exclude-drawing-sheet \
    --fit-page-to-board \
    -l "Edge.Cuts,F.Silkscreen,B.Silkscreen,B.Fab,F.Fab,B.Paste,F.Paste,F.Courtyard,F.Cu,In2.Cu,In3.Cu,In5.Cu,In6.Cu,In7.Cu,In8.Cu,B.Cu" \
    -o tp.svg

    inkscape --export-height=1024 --export-type=png tp.svg
}

export_top_view () {
    top_filename="render.png"
    kicad-cli pcb render tp.kicad_pcb \
        -o $top_filename \
        --width 650 \
        --preset follow_pcb_editor \
        --quality high \
        --use-board-stackup-colors \
        --perspective

    magick $top_filename -crop 616x872+87+0 $top_filename
    echo "Finished ${top_filename}"
}

export_side_view () {
    side_filename="render_side.png"
    kicad-cli pcb render tp.kicad_pcb \
        -o $side_filename \
        --quality high \
        --width 261 \
        --preset follow_pcb_editor \
        --use-board-stackup-colors \
        --rotate '0,300,0' \
        --perspective \
        --zoom 0.85

    magick $side_filename -crop 228x872+14+0 $side_filename
    echo "Finished ${side_filename}"
}


export_svg
export_top_view
export_side_view
