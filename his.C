{
h1 = new TH1D("h1", "One Dice;Roll;Counts", 6, 0.5, 6.5);
h2 = new TH1D("h2","Two Dices;Roll;Count", 12, 0.5, 12.5);

for(int i =0; i<10000; i++) {

double r1 = gRandom->Integer(6) + 1;
double r2 = gRandom->Integer(6) + 1;

h1->Fill(r1);
h2->Fill(r2);

h2->Fill(r1+r2);

}

c1 = new TCanvas("c1","Bin Size", 1000, 500);
c1->Divide(2,1);

c1->cd(1);
h1->SetFillColor(kPink -6);
h1->SetMinimum(0);
h1->Draw();

c1->cd(2);
h2->SetFillColor(kBlue);
h2->SetMinimum(0);
h2->Draw();

cout << "Counts for sum of two dices" << endl;

for(int i=1; i<=h2->GetNbinsX(); i++)  {

cout << i << "\t" << h2->GetBinContent(i) << endl;

}

}
