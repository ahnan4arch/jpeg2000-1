#include "main.h"

void add_tab( double* tab1, int N, const double* tab2, int M )
{
    assert( N == M );
    for ( int i = 0 ;
        i < N ;
        ++i)
    {
        tab1[i] += tab2[i];
    }   
}

void print_tab( const double* tab, int N )
{
    for( int i = 0 ; i < N; i++)
        cout << tab[i] << " | ";
    cout << '\n';
}


double* concat( const double* tab1, int N, const double* tab2, int M)
{
    double* new_tab = new double[N+M];
    for (int i = 0 ; 
         i < N+M; 
         i++)
    {
        if(i < N)
            new_tab[i] = tab1[i];
        else
            new_tab[i] = tab2[i-N];
    }
    return new_tab; 
}

double * copy( const double * tab, int N )
{
    double * tmp = new double[N];   
    for (int i = 0 ;
         i < N ;
         ++i )
    {
        tmp[i] = tab[i];
    }
    return tmp;
}

void interp2( double* sig, int N )
{
    for (int i = N/2 ; i > 0 ; i--)
    {
        sig[2*i] = sig[i];
        sig[i] = 0;
    }
}

void decim2( double* sig, int N )
{
    for (int i = 1 ; i <= N/2 ; i++)
    {
        sig[i] = sig[2*i];
        sig[2*i] = 0;
    }
}

int getIdx( int id, int N )
{
    if ( id < 0 )
        id = - (id%N);
    if ( id >= N )
        id = (N-2) - (id%N);
    return id;
}

double* conv( double* sig, int N, double* h, int K )
{
    double* new_sig = new double[N];
    int shift = K/2;
    for (int i = 0; i < N ; i++)
    {
        double sum = 0;
        for (int k = 0 ; k < K ; k++)
        {
            int index = i - (k - shift);
            sum += h[k] * sig[getIdx(index,N)];
        }
        new_sig[i] = sum;
    }
    return new_sig;
}

void analyse_haar( double** x, int p)
{
    assert( (p%2) == 0 && " p must be a power of 2 " );
    double h0[3] = { 1/sqrt(2), 1/sqrt(2), 0 };
    double h1[3] = { 1/sqrt(2), -1/sqrt(2), 0};
    double* xb = conv(*x, p, h0, 3);
    double* xh = conv(*x, p, h1, 3);
    decim2( xb, p );
    decim2( xh, p );
    *x = concat( xb, p/2, xh, p/2);
}

double* synthese_haar(double* x, int p)
{
    assert( (p%2) == 0 && " p must be a power of 2 " );
    double g0[3] = { 0, 1/sqrt(2), 1/sqrt(2) };
    double g1[3] = { 0, -1/sqrt(2), 1/sqrt(2) };
    double * xbd = new double[p];
    double * xhd = new double[p];
    for ( int i = 0 ;
          i < p ;
          ++i)
    {
        if ( i < (p/2))
        {
            xbd[i] = x[i];
        }
        else
        {
            xhd[i -(p/2)] = x[i];
        }
    }
    interp2( xbd, p);
    interp2( xhd, p);
    double* yb = conv( xbd, p , g0, 3);
    double* yh = conv( xhd, p , g1, 3);
    add_tab( yb, p, yh, p);
    return yb;
}

void analyse_97( double** x, int p )
{
    assert( (p%2) == 0 && " p must be a power of 2 " );
    double* _h0 = new double[9];
    double* _h1 = new double[9];

    // Filtre biorthogonal 9/7 _h0 (longueur 9)
    _h0[0]=0.037828455507;
    _h0[1]=-0.023849465019;
    _h0[2]=-0.110624404418;
    _h0[3]=0.377402855613;
    _h0[4]=0.852698679009;
    _h0[5]=0.377402855613;
    _h0[6]=-0.110624404418;
    _h0[7]=-0.023849465019;
    _h0[8]=0.037828455507;

    // Filtre biorthogonal 9/7 _h1 (longueur 9)
    _h1[0]=0.064538882629;
    _h1[1]=-0.040689417610;
    _h1[2]=-0.418092273222;
    _h1[3]=0.788485616406;
    _h1[4]=-0.418092273222;
    _h1[5]=-0.040689417610;
    _h1[6]=0.064538882629;
    _h1[7]=0.000000000000;
    _h1[8]=-0.000000000000;

    double* xb = conv(*x, p, _h0, 9);
    double* xh = conv(*x, p, _h1, 9);
    decim2( xb, p );
    decim2( xh, p );
    *x = concat( xb, p/2, xh, p/2);
}

double* synthese_97( double* x, int p )
{
    assert( (p%2) == 0 && " p must be a power of 2 " );
    double* _g0 = new double[7];
    double* _g1 = new double[11];   

    // Filtre biorthogonal 9/7 _g0 (longueur 7)
    _g0[0]=-0.064538882629;
    _g0[1]=-0.040689417610;
    _g0[2]=0.418092273222;
    _g0[3]=0.788485616406;
    _g0[4]=0.418092273222;
    _g0[5]=-0.040689417610;
    _g0[6]=-0.064538882629;

    // Filtre biorthogonal 9/7 _g1 (longueur 11)
    _g1[0]=0.000000000000;
    _g1[1]=-0.000000000000;
    _g1[2]=0.037828455507;
    _g1[3]=0.023849465019;
    _g1[4]=-0.110624404418;
    _g1[5]=-0.377402855613;
    _g1[6]=0.852698679009;
    _g1[7]=-0.377402855613;
    _g1[8]=-0.110624404418;
    _g1[9]=0.023849465019;
    _g1[10]=0.037828455507;

    double * xbd = new double[p];
    double * xhd = new double[p];
    
    for ( int i = 0; 
          i < p ;
          ++i)
    {
        if ( i < (p/2))
        {
            xbd[i] = x[i];
        }
        else
        {
            xhd[i - (p/2)] = x[i];
        }
    } 
    
    interp2( xbd, p);
    interp2( xhd, p);
    double* yb = conv( xbd, p , _g0, 7);
    double* yh = conv( xhd, p , _g1, 11); 
    add_tab( yb, p, yh, p);
    return yb;
}

double * get_signal_from_file( const char* filename )
{
    ifstream file(filename);
    std::string line;
    int size = 0;
    if(file.is_open())
    {
        while ( getline( file, line ) )
        {
            ++size;
        }
    }else{ cout << "Error opening file"; }  
    file.close();
    double * signal = new double[size];
    ifstream file2(filename);
    if(file2.is_open())
    {
        for (int i=0; i<size ; i++)
        {
            getline(file2, line);           
            signal[i] = atof( line.c_str());
        }
    }
    file2.close();
    return signal;
}

void write_signal_to_file( const char* filename, double* x, int N)
{
    ofstream file(filename);
    if(file.is_open())
    {
        for (int i=0; i<N ; i++)
        {
            file << x[i] << "\n";
        }   
    }
    file.close();
}

void analyse_97_lifting( double* x, int p )
{
    assert( (p%2) == 0 && " p must be a power of 2 " );
    double a = -1.586134342;
    for ( int i = 0;
          i < p/2;
          ++i)
    {
        x[ getIdx( (2*i+1) , p) ] = a * x[ getIdx( (2*i) , p ) ] + x[ getIdx( (2*i+1), p) ] + a * x[ getIdx( (2*i + 2) , p) ];
    }

    a = -0.05298011854;
    for ( int i = 0;
          i < p/2;
          ++i)
    {
        x[ getIdx( (2*i), p) ] = a * x[ getIdx( (2*i-1), p ) ] + x[ getIdx( (2*i) , p)] + a * x[ getIdx( (2*i+1), p ) ];
    }

    a = 0.8829110762;
    for ( int i = 0 ;
          i < p/2;
          ++i)
    {
        x[ getIdx( (2*i+1), p ) ] = a * x[ getIdx( (2*i), p ) ] + x[ getIdx( (2*i+1), p)] + a * x[ getIdx( (2*i+2), p )];
    }

    a = 0.4435068522;
    for ( int i = 0;
          i < p/2;
          ++i)
    {
        x[ getIdx( (2*i), p )] = a * x[ getIdx( (2*i-1), p )] + x[ getIdx( (2*i), p )] + a * x[ getIdx( (2*i+1), p )];

    }

    a = 1/1.149604398;
    for ( int i = 0 ;
          i < p/2;
          ++i )
    {
        x[ getIdx( (2*i), p )] /= a;
        x[ getIdx( (2*i+1), p )] *= a;
    }

    double * tmp = copy( x, p);
    int odd_idx = (p/2);
    int even_idx = 0;
    for ( int i = 0 ;
          i < p ;
          ++i )
    {
        if ( i%2 == 0 )
        {
            x[even_idx] = tmp[i];
            ++even_idx;
        }
        else
        {
            x[odd_idx] = tmp[i];
            ++odd_idx;
        }
    }

}

double * synthese_97_lifting( double* x, int p )
{
    assert( (p%2) == 0 && " p must be a power of 2 " );
    
    double * tmp = copy( x, p);
    int odd_idx = (p/2);
    int even_idx = 0;
    for ( int i = 0 ;
          i < p ;
          ++i )
    {
        if ( i%2 == 0 )
        {
            x[i] = tmp[even_idx];
            ++even_idx;
        }
        else
        {
            x[i] = tmp[odd_idx];
            ++odd_idx;
        }
    }

    double a = 1.149604398; 
    for ( int i = 0;
          i < p/2;
          ++i)
    {
        x[ getIdx( (2*i), p) ] /= a;
        x[ getIdx( (2*i+1), p)] *= a;       
    }

    a = -0.4435068522;
    for ( int i = 0;
          i < p/2;
          ++i)
    {
        x[ getIdx( (2*i), p)] = a * x[ getIdx( (2*i-1), p)] + x[ getIdx( (2*i), p )] + a * x[ getIdx( (2*i+1), p)];
    }

    a = -0.8829110762;
    for ( int i = 0 ;
          i < p/2;
          ++i)
    {
        x[ getIdx( (2*i+1), p)] = a * x[ getIdx( (2*i), p)] + x[getIdx( (2*i+1), p)] + a * x[getIdx( (2*i+2), p)];
    }

    a = 0.05298011854;
    for ( int i = 0;
          i < p/2;
          ++i)
    {
        x[ getIdx( (2*i), p)] = a * x[ getIdx( (2*i-1), p)] + x[ getIdx( (2*i), p)] + a * x[ getIdx( (2*i+1), p)];
    }

    a = 1.586134342;
    for ( int i = 0 ;
          i < p/2;
          ++i )
    {
        x[ getIdx( (2*i+1), p)] = a * x[ getIdx( (2*i), p)] + x[ getIdx( (2*i+1),p)] + a * x[ getIdx( (2*i + 2), p)];
    }

    return 0;
}

void amr (double* x, int p, int niveau )
{
    for ( int i = 1;
          i <= niveau;
          ++i )
    {
        analyse_97_lifting(x, p/i);
    }
}

void iamr (double* x, int p, int niveau )
{
    for ( int i = niveau;
          i > 0;
          --i )
    {
        synthese_97_lifting(x, p/i);
    }
}

double error(const double* tab1, int N, const double* tab2, int M)
{
    assert( N == M && "taille de tableau differentes");
    double e = 0;   
    for ( int i = 0; i < N; ++i)
    {
        e += (tab1[i] - tab2[i]) * (tab1[i] - tab2[i]);
    }
    return e;
}

void analyse2D_97( double* m, int p, int size)
{
    for ( int i = 0 ;
          i < p;
          ++i)
    {
        analyse_97_lifting( m +(i*size), p);
    }

    for ( int i = 0 ;
          i < p;
          ++i)
    {
        double * column = new double[p];
        for ( int j = 0;
              j < p;
              ++j)
        {
            column[j] = m[ i + (j*size)];
        }
        analyse_97_lifting( column, p);
        for ( int j = 0;
              j < p;
              ++j)
        {
            m[ i + (j*size)] = column[j];
        }        
    }
}

void synthese2D_97( double* m, int p, int size )
{
    for ( int i = 0 ;
          i < p;
          ++i)
    {
        double * column = new double[p];
        for ( int j = 0;
              j < p;
              ++j)
        {
            column[j] = m[ i + (j*size)];
        }
        synthese_97_lifting( column, p);
        for ( int j = 0;
              j < p;
              ++j)
        {
            m[ i + (j*size)] = column[j];
        }        
    }
    for ( int i = 0 ;
          i < p;
          ++i)
    {
        synthese_97_lifting( m+(i*size) , p );
    }
}

void amr2D_97( double* m, int p, int j)
{ 
    for( int i = 1;
         i <= j;
         ++i)
    {
        analyse2D_97( m, p/i, p);
    }
}

void iamr2D_97( double* m, int p, int j)
{
    for( int i = j;
         i > 0;
         --i)
    {
        synthese2D_97( m, p/i, p) ;
    }
}

double* charge_bmp256(const char* fichier, uint32_t* largeur, uint32_t* hauteur) {
    FILE* fp;
    uint16_t bfType;
    uint32_t bfOffBits;
    uint32_t biWidth;
    uint32_t biHeight;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint8_t *pixels;
    uint32_t pixelsSize;
    uint32_t x, y;
    double* m;

    fp = fopen(fichier, "rb");
    if (fp == NULL) {
        printf("charge_bmp256: impossible d'ouvrir le fichier %s en lecture !\n",
                fichier);
        return NULL;
    }

    // BMP specifications : http://members.fortunecity.com/shetzl/bmpffrmt.html
    // Lecture de l'entête
    fread(&bfType, sizeof(uint16_t), 1, fp);
    if (bfType != 19778) {
        printf("charge_bmp256: le fichier %s n'est pas un fichier BMP !\n",
                fichier);
        fclose(fp);
        return NULL;
    }

    // Lecture de l'offset du debut du bitmap
    fseek(fp, 10, SEEK_SET);
    fread(&bfOffBits, sizeof(uint32_t), 1, fp);

    // Lecture de la largeur et de la hauteur
    fseek(fp, 18, SEEK_SET);
    fread(&biWidth, sizeof(uint32_t), 1, fp);
    *largeur = (int) biWidth;
    fread(&biHeight, sizeof(uint32_t), 1, fp);
    *hauteur = (int) biHeight;

    // Verification que l'image est bien en mode 256 couleurs
    fseek(fp, 28, SEEK_SET);
    fread(&biBitCount, sizeof(uint16_t), 1, fp);
    if (biBitCount != 8) {
        printf(
                "charge_bmp256: le fichier BMP %s n'est pas en mode 256 couleurs !\n",
                fichier);
        fclose(fp);
        return NULL;
    }

    // Verification que l'image n'est pas compressée
    fseek(fp, 30, SEEK_SET);
    fread(&biCompression, sizeof(uint32_t), 1, fp);
    if (biCompression != 0) {
        printf("charge_bmp256: le fichier BMP %s est en mode compressé !\n",
                fichier);
        fclose(fp);
        return NULL;
    }

    // Allocation d'un bloc memoire pour lire les pixels et lecture de ceux-ci
    pixelsSize = (*largeur) * (*hauteur);
    pixels = (uint8_t *) malloc(pixelsSize);
    fseek(fp, bfOffBits, SEEK_SET);
    fread(pixels, pixelsSize, 1, fp);

    // Copie dans un buffer de double et transposition des lignes
    m = (double *) calloc(pixelsSize, sizeof(double));
    for (y = 0; y < *hauteur; y++) {
        for (x = 0; x < *largeur; x++) {
            m[x + *largeur * (*hauteur - 1 - y)] = (double) pixels[x + *largeur
                    * y];
        }
    }
    free(pixels);

    fclose(fp);

    return m;
}

int ecrit_bmp256(const char* fichier, uint32_t largeur, uint32_t hauteur, double* m) {
    FILE* fp;
    uint16_t us;
    uint32_t ul;
    uint8_t uc;
    uint32_t i;
    uint32_t pixelsSize;
    uint32_t x, y;
    uint8_t* pixels;

    fp = fopen(fichier, "wb");
    if (fp == NULL) {
        printf("ecrit_bmp256: impossible d'ouvrir le fichier %s en écriture !\n",
                fichier);
        return 0;
    }

    pixelsSize = largeur * hauteur;

    // Conversion double => uint8_t
    pixels = (uint8_t *) malloc(pixelsSize);
    for (y = 0; y < hauteur; y++) {
        for (x = 0; x < largeur; x++) {
            double d;
            uint8_t c;
            d = m[x + largeur * y];
            if (d < 0.0)
                c = 0;
            else if (d > 255.0)
                c = 255;
            else
                c = (uint8_t) d;

            pixels[x + largeur * (hauteur - 1 - y)] = c;
        }
    }

    // Ecriture de l'entête standard
    // bfType
    us = 19778;
    fwrite(&us, sizeof(uint16_t), 1, fp);

    // bfSize
    // taille image + taille BITMAPFILEHEADER + taille BITMAPINFOHEADER + taille palette
    ul = pixelsSize + 14 + 40 + 256 * 4;
    fwrite(&ul, sizeof(uint32_t), 1, fp);

    // bfReserved
    ul = 0;
    fwrite(&ul, sizeof(uint32_t), 1, fp);

    // bfOffBits
    // taille BITMAPFILEHEADER + taille BITMAPINFOHEADER + taille palette
    ul = 14 + 40 + 256 * 4;
    fwrite(&ul, sizeof(uint32_t), 1, fp);

    // biSize
    ul = 40;
    fwrite(&ul, sizeof(uint32_t), 1, fp);

    // biWidth
    ul = largeur;
    fwrite(&ul, sizeof(uint32_t), 1, fp);

    // biHeight
    ul = hauteur;
    fwrite(&ul, sizeof(uint32_t), 1, fp);

    // biPlanes
    us = 1;
    fwrite(&us, sizeof(uint16_t), 1, fp);

    // biBitCount
    us = 8;
    fwrite(&us, sizeof(uint16_t), 1, fp);

    // biCompression
    ul = 0;
    fwrite(&ul, sizeof(uint32_t), 1, fp);

    // biSizeImage
    ul = pixelsSize;
    fwrite(&ul, sizeof(uint32_t), 1, fp);

    // biXPelsPerMeter
    ul = 0;
    fwrite(&ul, sizeof(uint32_t), 1, fp);

    // biYPelsPerMeter
    ul = 0;
    fwrite(&ul, sizeof(uint32_t), 1, fp);

    // biClrUsed
    ul = 0;
    fwrite(&ul, sizeof(uint32_t), 1, fp);

    // biClrImportant
    ul = 0;
    fwrite(&ul, sizeof(uint32_t), 1, fp);

    // Ecriture de la palette en niveaux de gris
    for (i = 0; i < 256; i++) {
        uc = i;
        fwrite(&uc, sizeof(uint8_t), 1, fp);

        uc = i;
        fwrite(&uc, sizeof(uint8_t), 1, fp);

        uc = i;
        fwrite(&uc, sizeof(uint8_t), 1, fp);

        uc = 0;
        fwrite(&uc, sizeof(uint8_t), 1, fp);
    }

    // Ecriture de l'image
    fwrite(pixels, largeur * hauteur, 1, fp);

    free(pixels);

    fclose(fp);
    return 1;
}

double mean_band( double * m, int p)
{
    double res = 0.0f;
    for ( int row = 0;
          row < p;
          ++row)
    {
        for (int col = 0;
             col < p;
             ++col)
        {
            res += m[ col + (p*row) ];
        }
    }
    return (res / (p*p));
}

double var_band( double* m, int p, double mean)
{
    double res = 0.0f;
    for ( int row = 0;
          row < p;
          ++row)
    {
        for (int col = 0;
             col < p;
             ++col)
        {
            res += (m[ col + (p*row) ] - mean)*(m[ col + (p*row) ] - mean);
        }
    }
    return ((1.0f / (p*p)) * (res));
}

double* extract_band( double* m, int p, int size)
{
    double* band = new double[p*p];
    for(int row = 0;
        row < p;
        ++row)
    {
        for(int col = 0;
            col < p;
            ++col)
        {
            band[ col + (row*p)] = m[ col + (row*size)];
        }
    }
    return (band);
}

double* compute_means( double* m, int p, int j )
{
    double* means = new double[ 3*j +1 ];
    double* band;
    int width;
    width = (p/(2*j));
    band = extract_band( m, width, p);
    means[ 3*j ] = mean_band( band, width);
    for ( int i = j;
          i > 0;
          --i)
    {
        width = (p/(2*i));
        band = extract_band( m + width, width, p);
        means[ 3*j-1 ] = mean_band(band,width);
        band = extract_band( m + (p*width), width, p);
        means[ 3*j -2 ] = mean_band(band,width);
        band = extract_band( (m + width)+(p*width), width, p);
        means[ 3*(j-1) ] = mean_band(band,width);     
    }
    return(means);
}

double* compute_vars( double* m, int p, int j, double* means )
{
    double* vars = new double[ 3*j +1 ];
    double* band;
    int width;
    width = (p/(2*j));
    band = extract_band( m, width, p);
    vars[ 3*j ] = var_band( band, width, means[3*j]);
    for ( int i = j;
          i > 0;
          --i)
    {
        width = (p/(2*i));
        band = extract_band( m + width, width, p);
        vars[ 3*j-1 ] = var_band(band,width,means[3*j-1]);
        band = extract_band( m + (p*width), width, p);
        vars[ 3*j -2 ] = var_band(band,width, means[3*j-2]);
        band = extract_band( (m + width)+(p*width), width, p);
        vars[ 3*(j-1) ] = var_band(band,width, means[3*(j-1)]);     
    }
    return(vars);
}

int main()
{
    int N = 4096;
    uint32_t size = 256;
    uint32_t* size_t = &size;

    double* lena = charge_bmp256("lena.bmp" , size_t, size_t );
    amr2D_97 ( lena, size, 1);
    double* means = compute_means( lena, size, 1);
    double* vars = compute_vars( lena, size, 1, means);
    print_tab( means, 4 );
    print_tab( vars, 4 );

    double * isig_haar = get_signal_from_file( "leleccum.txt" );
    analyse_haar( &isig_haar, N );
    double * osig_haar = synthese_haar( isig_haar, N ); 
    write_signal_to_file( "osig_haar.txt" , osig_haar, N);

    double * isig_97 = get_signal_from_file( "leleccum.txt" );
    analyse_97( &isig_97, N );
    double * osig_97 = synthese_97( isig_97, N );
    write_signal_to_file( "osig_97.txt" , osig_97, N);

    double * isig_lift = get_signal_from_file( "leleccum.txt" );
    analyse_97_lifting( isig_lift, N);
    synthese_97_lifting( isig_lift, N);
    write_signal_to_file( "osig_lift.txt", isig_lift, N);

    double * isig_amr = get_signal_from_file( "leleccum.txt" );
    amr(isig_amr, N, 2);
    iamr( isig_amr, N, 2);
    write_signal_to_file( "osig_amr.txt", isig_amr, N);

    amr2D_97( lena, size, 2);
    iamr2D_97( lena, size, 2);
    ecrit_bmp256( "output.bmp", size, size, lena);
    
    double * groundTruthSig = get_signal_from_file( "leleccum.txt" );
    double e_haar = error(groundTruthSig, N, osig_haar, N);
    double e_97 = error(groundTruthSig, N, osig_97, N);
    double e_lift = error(groundTruthSig, N, isig_lift, N);
    double e_amr = error( groundTruthSig, N, isig_amr, N);
    cout << "\n Error haar : " << e_haar << "\n Error 97 : " << e_97 << "\n Error lift : " << e_lift << "\n Error AMR : " << e_amr << endl ;
}