client_nodes=(n0 n1 n2 n3 n4 n5 n6 n7 n8 n9 n10 n11 n12 n13 n14 n15 n16 n17 n18 n19 n20 n21 n22 n23 n24 n25 n26)

SCHEME=$1
NUMCLIENTS=10


for ((i=0;i<$NUMCLIENTS;i++)); do
    n=${client_nodes[$i]}
    scp ${n}:/tmp/2D/run_expt/${n}_${SCHEME}_fct /tmp/2D/run_expt/
done

cat *_${SCHEME}_* > scheme=${SCHEME}_fct
