
{
const Int_t v = 1023;

Double_t x[v], y[v];


for (int i = 0; i<v; i++)   
{
x[i] = timeV[1][i];
y[i] = (waveform[1][i]*(-1));
}

Double_t max=y[0];
Double_t maxx=x[0];


for(int i = 1; i < v; i++)
{
if( y[i] > max )
{
max = y[i + 1];
maxx= x[i + 1];
}

}

double zero = 0;

for (int i = 0; i<40; i++)
zero=+y[i];

zero=zero/40;
double maximum = maxx - zero;

cout << "zero: " << zero << endl;
cout << "Maksymalna wartosc: " << max << endl;
cout << "Maksimum: " << maximum << endl;

 f = new TF1("f", "gaus", (maximum-10), (maximum+10));

 TGraph *g;
 g = new TGraph(v, x, y);

 g->Fit(f);
 g->Draw("AL");
}
