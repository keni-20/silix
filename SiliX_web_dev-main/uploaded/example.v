module c17 ();

input N1, N2, N3;
output N6, N22, N23, N6;

wire w1, w2, w3, w4;

nand ND2_1 ( w1, N1, N3);
and AND2_2 ( w2, N2, N3);
nand ND2_3 ( N6, w1, w2);
or OR2_4  ( w3, N1, N3);
or OR2_5  ( w4, w3, N2);
xor XOR2_6 ( N22, N1, N2);
xnor XNOR2_7 ( N23, N1, N2);

endmodule
