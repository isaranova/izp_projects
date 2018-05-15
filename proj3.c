/**
 *@file proj3.c
 *@author Ivana Saranova <xsaran02@stud.fit.vutbr.cz>
 *@date 3.12.2017
 *@name 3. projekt do predmetu IZP: Jednoducha shlukova analyza (Unweighted pair-group average)
 *@see https://is.muni.cz/th/172767/fi_b/5739129/web/web/usrov.html
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h> // sqrtf
#include <limits.h> // INT_MAX
#include <string.h> //pro strcmp

/*****************************************************************
 * Ladici makra. Vypnout jejich efekt lze definici makra
 * NDEBUG, napr.:
 *   a) pri prekladu argumentem prekladaci -DNDEBUG
 *   b) v souboru (na radek pred #include <assert.h>
 *      #define NDEBUG
 */
#ifdef NDEBUG
#define debug(s)
#define dfmt(s, ...)
#define dint(i)
#define dfloat(f)
#else

// vypise ladici retezec
#define debug(s) printf("- %s\n", s)

// vypise formatovany ladici vystup - pouziti podobne jako printf
#define dfmt(s, ...) printf(" - "__FILE__":%u: "s"\n",__LINE__,__VA_ARGS__)

// vypise ladici informaci o promenne - pouziti dint(identifikator_promenne)
#define dint(i) printf(" - " __FILE__ ":%u: " #i " = %d\n", __LINE__, i)

// vypise ladici informaci o promenne typu float - pouziti
// dfloat(identifikator_promenne)
#define dfloat(f) printf(" - " __FILE__ ":%u: " #f " = %g\n", __LINE__, f)

#endif

/*****************************************************************
 * Deklarace potrebnych datovych typu:
 *
 * TYTO DEKLARACE NEMENTE
 *
 *   struct obj_t - struktura objektu: identifikator a souradnice
 *   struct cluster_t - shluk objektu:
 *      pocet objektu ve shluku,
 *      kapacita shluku (pocet objektu, pro ktere je rezervovano
 *          misto v poli),
 *      ukazatel na pole shluku.
 */

struct obj_t {
    int id; /**< uklada identifikacni cislo objektu */
    float x; /**< uklada prvni souradnici objektu */
    float y; /**< uklada druhou souradnici objektu */
};

struct cluster_t {
    int size; /**< uklada pocet objektu ve strukture */
    int capacity; /**< uklada maximalni pocet objektu ve strukture */
    struct obj_t *obj; /** ukazatel na pole objektu */
};

/*****************************************************************
 * Deklarace potrebnych funkci.
 *
 * PROTOTYPY FUNKCI NEMENTE
 *
 * IMPLEMENTUJTE POUZE FUNKCE NA MISTECH OZNACENYCH 'TODO'
 *
 */

/**
 *@brief Inicializace shluku 'c'
 * Alokuje pamet pro cap objektu (kapacitu).
 * Ukazatel NULL u pole objektu znamena kapacitu 0.
 *@param c - shluk k inicializaci
 *@param cap - kapacita shluku
 */
void init_cluster(struct cluster_t *c, int cap)
{
    assert(c != NULL);
    assert(cap >= 0);

    //nastaveni poctu objektu na 0 a capacitu na cap
    c->size = 0;
    c->capacity = cap;

    if(cap > 0) //kontrola NULL hodnoty v load_clusters
        c->obj = malloc(cap* sizeof(struct obj_t));
    if(c->obj == NULL || c->capacity == 0)
    {
        c->capacity = 0;
        c->obj = NULL;
    }
}

/**
 *@brief Odstraneni objektu shluku
 * Odstraneni vsech objektu shluku a inicializace na prazdny shluk.
 *@param c - shluk k odstraneni
 */
void clear_cluster(struct cluster_t *c)
{
    free(c->obj);
    init_cluster(c, 0);
}

/// Chunk of cluster objects. Value recommended for reallocation.
const int CLUSTER_CHUNK = 10;

/**
 *@brief Zmena kapacity shluku
 * Zmena kapacity shluku 'c' na kapacitu 'new_cap'.
 *@param c - shluk ke zmene kapacity
 *@param new_cap - nova kapacita shluku
 *@return shluk o nove kapacite, nebo pokud nastane chyba, tak NULL
 */
struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap)
{
    // TUTO FUNKCI NEMENTE
    assert(c);
    assert(c->capacity >= 0);
    assert(new_cap >= 0);

    if (c->capacity >= new_cap)
        return c;

    size_t size = sizeof(struct obj_t) * new_cap;

    void *arr = realloc(c->obj, size);
    if (arr == NULL)
        return NULL;

    c->obj = (struct obj_t*)arr;
    c->capacity = new_cap;
    return c;
}

/**
 *@brief Pridani objektu na konec shluku
 * Prida objekt 'obj' na konec shluku 'c'.
 * Rozsiri shluk, pokud se do nej objekt nevejde.
 *@param c - shluk, kam se prida objekt
 *@param obj - objekt, ktery bude pridan do shluku
 */
void append_cluster(struct cluster_t *c, struct obj_t obj)
{
  if (c->size >= c->capacity)
      resize_cluster(c, c->capacity+CLUSTER_CHUNK);

  c->obj[c->size] = obj; //pridani objektu na posledni pozici
  c->size++; //zvetseni poctu objektu o 1
}

/**
 *@brief Serazeni shluku
 * Seradi objekty ve shluku 'c' vzestupne podle jejich identifikacniho cisla.
 *@param c - shluk, ktery bude serazen
 */
void sort_cluster(struct cluster_t *c);

/**
 *@brief Slouceni 2 shluku
 * Do shluku 'c1' prida objekty 'c2'.
 * Shluk 'c1' bude v pripade nutnosti rozsiren.
 * Objekty ve shluku 'c1' budou serazeny vzestupne podle identifikacniho cisla.
 * Shluk 'c2' bude nezmenen.
 *@param c1 - shluk, do ktereho budou pridany vsechny objekty z druheho shluku
 *@param c2 - shluk, jehoz objekty budou pridany do prvniho shluku
 */
void merge_clusters(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c2 != NULL);

    for(int i = 0; i < c2->size; i++)
        append_cluster(c1, c2->obj[i]);

    sort_cluster(c1);
}

/**********************************************************************/
/* Prace s polem shluku */

int premium_case = 2; //povolena globalni promena ukladajici typ metody shlukovani

/**
 *@brief Odstraneni shluku
 * Odstrani shluk z pole shluku 'carr'.
 * Pole shluku obsahuje 'narr' polozek (shluku).
 * Shluk pro odstraneni se nachazi na indexu 'idx'.
 * Funkce vraci novy pocet shluku v poli.
 *@param carr - pole shluku z neho bude shluk odstranen
 *@param narr - pocet shluku v poli shluku
 *@param idx - index shluku urceneho k odstraneni
 *@return novy pocet shluku v poli shluku carr
*/
int remove_cluster(struct cluster_t *carr, int narr, int idx)
{
    assert(idx < narr);
    assert(narr > 0);

    struct cluster_t tmp;

    for(int i = idx; i < narr-1; i++)
    {
        tmp = carr[i];
        carr[i] = carr[i+1];
        carr[i+1] = tmp;
    }

    clear_cluster(&carr[narr-1]);
    return narr-1;
}

/**
 *@brief Vzdalenost mezi objekty
 * Pocita Euklidovskou vzdalenost mezi dvema objekty.
 *@param o1 - prvni objekt pro mereni vzdalenosti
 *@param o2 - druhy objekt pro mereni vzdalenosti
 *@return vzdalenost mezi objekty o1 a o2
 */
float obj_distance(struct obj_t *o1, struct obj_t *o2)
{
    assert(o1 != NULL);
    assert(o2 != NULL);

    float dist = sqrtf((o2->x - o1->x)*(o2->x - o1->x) + (o2->y - o1->y)*(o2->y - o1->y));
    return dist;
}

/**
 *@brief Vzdalenost 2 shluku
 * Pocita vzdalenost dvou shluku pomoci metody unweighted pair-group average.
 *@param c1 - prvni shluk pro mereni vzdalenosti
 *@param c2 - druhy shluk pro mereni vzdalenosti
 *@return  vzdalenost mezi shluky c1 a c2
*/
float cluster_distance(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c1->size > 0);
    assert(c2 != NULL);
    assert(c2->size > 0);

    float avdist = 0.0;
    float bdist = -INT_MAX;
    float sdist = INT_MAX;

		//tvori kombinace mezi objekty shluku a vytvori jejich prumernou hodnotu
    for(int i = 0; i < c1->size; i++)
        for(int j = 0; j < c2->size; j++)
        {
            avdist += obj_distance(&c1->obj[i], &c2->obj[j]);
            if(obj_distance(&c1->obj[i], &c2->obj[j]) > bdist)
                bdist = obj_distance(&c1->obj[i], &c2->obj[j]);
            if(obj_distance(&c1->obj[i], &c2->obj[j]) < sdist)
                sdist = obj_distance(&c1->obj[i], &c2->obj[j]);
        }

    if(premium_case == 2)
        return avdist/(c1->size * c2->size);
    if(premium_case == 1)
        return bdist;
    return sdist;
}

/**
 *@brief Nalezeni dvou nejblizsich shluku
 * Funkce najde dva nejblizsi shluky.
 * V poli shluku 'carr' o velikosti 'narr' hleda dva nejblizsi shluky.
 * Nalezene shluky identifikuje jejich indexy v poli 'carr'.
 * Funkce nalezene shluky (indexy do pole 'carr') uklada do pameti na adresu 'c1' resp. 'c2'.
 *@param carr - pole shluku mezi jehoz shluky hledame dva nejblizsi
 *@param narr - pocet shluku v poli shluku
 *@param c1 - index prvniho z dvou nejblizsich shluku
 *@param c2 - index druheho z dvou nejblizsich shluku
*/
void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2)
{
    assert(narr > 0);

    float sdist = INT_MAX;

		//prochazi matici "kazdy s kazdym" nad diagonalou bez prvku diagonaly
    for(int i = 0; i < narr-1; i++)
        for(int j = i+1; j<narr; j++)
            if(cluster_distance(&carr[i], &carr[j]) < sdist)
            {
                 sdist = cluster_distance(&carr[i], &carr[j]);
                 *c1 = i;
                 *c2 = j;
            }
}

/**
 *@brief Pomocna funkce pro razeni shluku
 *@param a - objekt o1
 *@param b - objekt o2
 *@return -1 pokud je id o1 mensi nez id o2, 1 pokud je id o1 vetsi nez id o2, 0 pokud jsou si rovny
*/
static int obj_sort_compar(const void *a, const void *b)
{
    // TUTO FUNKCI NEMENTE
    const struct obj_t *o1 = (const struct obj_t *)a;
    const struct obj_t *o2 = (const struct obj_t *)b;
    if (o1->id < o2->id) return -1;
    if (o1->id > o2->id) return 1;
    return 0;
}

/**
 *@brief Serazeni objektu
 * Razeni objektu ve shluku vzestupne podle jejich identifikatoru.
 *@param c - shluk ke setrideni
*/
void sort_cluster(struct cluster_t *c)
{
    // TUTO FUNKCI NEMENTE
    qsort(c->obj, c->size, sizeof(struct obj_t), &obj_sort_compar);
}

/**
 *@brief Tisk shluku 'c' na stdout.
 *@param c - shluk, ktery bude vytisten
*/
void print_cluster(struct cluster_t *c)
{
    // TUTO FUNKCI NEMENTE
    for (int i = 0; i < c->size; i++)
    {
        if (i) putchar(' ');
        printf("%d[%g,%g]", c->obj[i].id, c->obj[i].x, c->obj[i].y);
    }
    putchar('\n');
}

/**
 *Nacteni shluku ze souboru
 * Ze souboru 'filename' nacte objekty.
 * Pro kazdy objekt vytvori shluk a ulozi jej do pole shluku.
 * Alokuje prostor pro pole vsech shluku a ukazatel na prvni
 * polozku pole (ukalazatel na prvni shluk v alokovanem poli) ulozi do pameti,
 * kam se odkazuje parametr 'arr'.
 * Funkce vraci pocet nactenych objektu (shluku).
 * V pripade nejake chyby uklada do pameti, kam se odkazuje 'arr', hodnotu NULL.
 *@param filename - pole znaku s nazvem souboru, ktery bude otevren
 *@param arr - ukazatel na pole shluku, do ktereho budou ulozeny shluky s objekty
 *@return pocet nactenych shluku, pokud nastane chyba -1
*/
int load_clusters(char *filename, struct cluster_t **arr)
{
    assert(arr != NULL);

    //deklarace pomocnych promennych
    char count[7]; //promenna pro count
    int cap = 0; //pocet nactenych shluku
    char end;

    //otevreni souboru
    FILE* file;
    file = fopen(filename, "r");
    if(file == NULL)
    {
        fprintf(stderr, "Error - file could not be opened!\n");
        *arr = NULL;
        return -1;
    }

    //nacteni prvniho radku - ocekavan format "count=xx"
    int c = fscanf(file, "%6s%d%c", count,&cap,&end);
    if(c != 3 || cap < 0)
    {
        fprintf(stderr, "Error - wrong count format!\n");
        *arr = NULL;
        return -1;
    }

		//vynulovani pole pro ukladani id hodnot
    int id_array[cap];
    for(int i = 0; i <cap; i++)
        id_array[i] = '\0';

    //alokovani pameti pro pole shluku
    *arr = malloc(cap*sizeof(struct cluster_t));
    struct cluster_t *clusters = *arr; //pomocnĂŠ pole struktur;
    if(*arr == NULL)
    {
        fprintf(stderr,"Error - memory could not be allocated!\n");
        *arr = NULL;
        return -1;
    }

    //nacitani jednotlivych radku souboru s objekty
    for(int i = 0; i < cap; i++)
    {
      struct obj_t obj; //pomocnĂĄ struktura pro objekt

      int n = fscanf(file," %d %f %f", &obj.id, &obj.x, &obj.y);
      if(n == 3 && n != EOF )
      {
					//kontrola, zda je identifikator jednoznacny
          for(int j = 0; id_array[j] != '\0'; j++)
              if(id_array[j] == obj.id)
              {
                  fprintf(stderr, "Error - object identifier must be unique!\n");
                  for(int k = 0; k < i; k++)
                    clear_cluster(&clusters[k]);
                  free(*arr);
                  *arr = NULL;
                  fclose(file);
                  return -1;
              }
          id_array[i] = obj.id;

					//kontrola, zda je pouze jeden objekt na radku
          char line;
          fscanf(file, "%c", &line);
          if(line == '\n' || line == EOF)
          {
              if(obj.x < 0.0 || obj.x >1000.0 || obj.y < 0.0 || obj.y > 1000.0
              || obj.x-(int)obj.x !=0 || obj.y-(int)obj.y !=0)
              {
                  fprintf(stderr, "Error - object's coordinates must be integer higher or equal to 0 and lower or equal to 1000!\n");
                  for(int k = 0; k < i; k++)
                      clear_cluster(&clusters[k]);
                  free(*arr);
                  *arr = NULL;
                  fclose(file);
                  return -1;
              }

							//inicializovani shluku
              init_cluster(&clusters[i], 1);
              if(clusters[i].obj == NULL)
              {
                  fprintf(stderr, "Error - memory could not be allocated!\n");
                  for(int k = 0; k < i; k++)
                      clear_cluster(&clusters[k]);
                  free(*arr);
                  *arr = NULL;
                  fclose(file);
                  return -1;
              }

							//pridani objektu do shluku
              append_cluster(&clusters[i], obj);
          }
          else
          {
              fprintf(stderr,"Error - only one object on line allowed!\n");
              for(int k = 0; k < i; k++)
                  clear_cluster(&clusters[k]);
              free(*arr);
              *arr = NULL;
              fclose(file);
              return -1;
          }
      }
      else
      {
          fprintf(stderr, "Error - not enough objects with proper format!\n");
          for(int k = 0; k < i; k++)
              clear_cluster(&clusters[k]);
          free(*arr);
          *arr = NULL;
          fclose(file);
          return -1;
      }
    }

    //zavreni souboru
    if(fclose(file) == EOF)
    {
        fprintf(stderr, "Error - file could not be closed!\n");
        for(int k = 0; k < cap; k++)
            clear_cluster(&clusters[k]);
        free(*arr);
        *arr = NULL;
        return cap;
    }

    return cap;
}

/**
 *@brief Tisk pole shluku
 * Parametr 'carr' je ukazatel na prvni polozku (shluk).
 * Tiskne se prvnich 'narr' shluku.
 *@param carr - pole shluku, jehoz sluky budou tisteny
 *@param narr - pocet shluku v poli shluku
*/
void print_clusters(struct cluster_t *carr, int narr)
{
    printf("Clusters:\n");
    for (int i = 0; i < narr; i++)
    {
        printf("cluster %d: ", i);
        print_cluster(&carr[i]);
    }
}

int main(int argc, char *argv[])
{
    struct cluster_t *clusters;

    if(argc > 1 && argc < 5)
    {
        //nacteni shluku do pole shluku clusters
        int count = load_clusters(argv[1], &clusters);
        if(clusters == NULL)
            return -1;

        int nclust = 1; //implicitni hodnota pozadovaneho poctu shluku
        int tempcount = count; //ulozeni puvodni hodnoty poctu shluku
        int strcount = 0; //pocet porovnavani u strcmp druheho nebo tretiho argumentu

        //nacteni volitelneho argumentu s pozadovanym poctem shluku
        if(argc >= 3 && argc <= 4)
        {
            nclust = atoi(argv[2]);
            if(nclust < 1)
            {
                fprintf(stderr, "Error - second argument must be integer higher than 0!\n");
                //uvolneni pameti i v pripade chyby
                for(int i = 0; i < tempcount; i++)
                    clear_cluster(&clusters[i]);
                free(clusters);
                return -1;
            }
            if(nclust > count)
            {
                fprintf(stderr, "Error - not enough objects in file for %d clusters!\n", nclust);
                //uvolneni pameti i v pripade chyby
                for(int i = 0; i < tempcount; i++)
                    clear_cluster(&clusters[i]);
                free(clusters);
                return -1;
            }
            if(argc == 4)
            {
                if(strcmp(argv[3], "--avg") == 0)
                {
                    premium_case = 2;
                    strcount = 1;
                }
                if(strcmp(argv[3], "--min") == 0)
                {
                    premium_case = 0;
                    strcount = 1;
                }
                if(strcmp(argv[3], "--max") == 0)
                {
                    premium_case = 1;
                    strcount = 1;
                }
            }
            else strcount = 1;
        }

        if(strcount != 1 && argc != 2)
        {
            fprintf(stderr, "Error - wrong use of third parameter! (use --avg, --min or --max)\n");
            //uvolneni pameti i v pripade chyby
            for(int i = 0; i < tempcount; i++)
                clear_cluster(&clusters[i]);
            free(clusters);
            return -1;
        }

        //prubeh samotne shlukove analyzy az na pozadovany pocet shluku
        while(count != nclust)
        {
            int id1, id2; //indexy shluku pro funkci find_neighbours
            find_neighbours(clusters, count, &id1, &id2);
            merge_clusters(&clusters[id1], &clusters[id2]);
            count = remove_cluster(clusters, count, id2);
        }

        print_clusters(clusters, count);

        //uvolneni pameti
        for(int i = 0; i < tempcount; i++)
            clear_cluster(&clusters[i]);
        free(clusters);
    }
    else
    {
        fprintf(stderr, "Error - wrong arguments!\n");
        return -1;
    }

    return 0;
}

