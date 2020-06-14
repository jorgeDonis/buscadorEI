RUTA_TREC_EVAL="/home/jorge/Desktop/EI/practica/buscadorEI/resources/materiales_buscador/trec_eval_8_1_linux"

for i in *.sal; do
    [ -f "$i" ] || break
    $RUTA_TREC_EVAL/trec_eval -q -o $RUTA_TREC_EVAL/frelevancia_trec_eval_TIME.txt $i > $i.precision_cobertura.res
done