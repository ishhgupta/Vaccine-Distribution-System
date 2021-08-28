#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

void red() {printf("\033[1;31m");}
void green() {printf("\033[1;32m");}
void yellow() {printf("\033[1;33m");}
void blue() {printf("\033[1;34m");}
void magenta() {printf("\033[1;35m");}
void cyan() {printf("\033[1;36m");}
void reset() {printf("\033[0m");}

int genRandomInRange(int l, int r) {
    return rand() % (r - l + 1) + l;
}
int test(int a)
{
    int m = rand()%100;
    if(m>=a) return 0;
    else return 1;
}
int min(int a,int b,int c)
{
    if(b>=a && c>=a) return a;
    else if(a>=b && c>=b) return b;
    else return c;
}

int no_company,no_zones,no_students;
typedef struct company{
    int id; 
    int status;             //if state=0 then doing nothing and if 1 then preparing vaccine
    int batchesLeft;        // no. of batches_prod left which a company produced
    int batchcapacity;           // capacity to vaccinate no. of students by a batch
    int batchesUsed;
    float prob;
    pthread_t thread_id;
}company;

typedef struct zone
{
    int id;
    int status;             // 0 if it is empty and 1 if it has batch of vaccine
    int whichcomp;
    int capacity;            //no. of vaccines a zone has
    pthread_t thread_id;
    pthread_mutex_t lock_zone; 
}zone;

typedef struct student
{
    int id;
    int status;             //if 1 then waiting or ready to get vaccined else 0
    int vaccinationno;       // the vaccination no. i.e. 1st 2nd or 3rd
    int ifvaccined;         // result of vaccination if vaccined or not
    int zone;               // in which zone the student is 
    int antibodytest;      // if anitbody test  
    pthread_t thread_id;   
}student;

pthread_mutex_t mutex;
company *companies; zone *zones; student *students; 
int students_waiting,stud_rem;

void distributing_vacc(company *comp)
{
    for(int i=1;i<=no_zones;i=i%no_zones+1)
    {
        if (comp -> batchesLeft!=0)
        {
            pthread_mutex_lock(&zones[i].lock_zone);
            if(zones[i].capacity<=0 && zones[i].status == 0)
            {
                comp->batchesLeft --;
                cyan(); printf("Pharmaceutical Company %d is delivering a vaccine batch to Vaccination Zone %d which has success probability Xj\n",comp->id,i);
                sleep(2);
                cyan(); printf("Pharmaceutical Company %d has delivered vaccines to Vaccination zone %d, resuming vaccination now.\n",comp->id,zones[i].id);
                zones[i].status=1;  //it is filled
                zones[i].capacity = comp->batchcapacity;
                zones[i].whichcomp = comp->id;
            }
        }
            if(comp -> batchesLeft == 0)
            {
                cyan(); printf("Pharmaceutical Company %d has distributed all the vaccines which it produced. Waiting to be used by zones\n",comp->id);
                pthread_mutex_unlock(&zones[i].lock_zone);
                break;
            }
            pthread_mutex_unlock(&zones[i].lock_zone);
        
    }
    while (comp->batchesUsed!=0)
    {
        
        // busy waiting
    }
    cyan(); printf("All the vaccines prepared by Pharmaceutical Company %d are emptied.Resuming Production Now.\n",comp->id);
    // return NULL;
}

void* prep_vaccine(void *args)
{
    company *comp = (company *)args;
    while (1)
    {
        // printf("hiiiiiiii\n");
        comp -> status=1;      //preparing a vaccine
        int batches_prod = genRandomInRange(1,5);
        int w = genRandomInRange(2,5);
        green(); printf("Pharmaceutical Company %d is preparing %d batches of vaccines which have success probability %f\n",comp->id,batches_prod,comp->prob);
        sleep(w);
        green(); printf("Pharmaceutical Company %d has prepared %d batches of vaccines which have success probability %f.\n Waiting for all the vaccines to be used to resume production.\n",comp->id,batches_prod,comp->prob);
        comp-> batchcapacity = genRandomInRange(10,20);
        comp-> batchesLeft = batches_prod;
        comp-> batchesUsed = batches_prod;
        distributing_vacc(comp);
    }
    return NULL;   
}

void* students_coming(void *args)
{
    student *stud = (student *)args;
    while(1)
    {
        // if(students[].)
        if(stud_rem > 0 && students[stud->id].ifvaccined ==0 && students[stud->id].vaccinationno<3)
        {
            blue(); printf("Student %d has arrived for %d round of Vaccination\n",stud->id,stud->vaccinationno);
            blue(); printf("Student %d is waiting to be allocated a slot on a Vaccination Zone\n",stud->id);
            sleep(4);
            students[stud->id].status = 1;
            students_waiting++;
            while (students[stud->id].status==1){ }
            students[stud->id].antibodytest = 0;
            while (students[stud->id].antibodytest==0){}
            if(students[stud->id].ifvaccined==1){break;}      
        }
        else if(students[stud->id].vaccinationno>=3)
        {
            if(stud_rem==0)
            {
                red(); printf("Simulation over\n");
                exit(0);
            }
            break;
        }
    }
    return NULL;
}

void* vaccinating(void* args){
    zone* zon = (zone*)args;
    int slots_num;
    while(1)
    {
        while (zon->status==0){ }
        int i;
        pthread_mutex_lock(&mutex);
        if(students_waiting>0 && zon->capacity>0)
        {
            slots_num = genRandomInRange(1,min(8,students_waiting,zon->capacity));
            yellow(); printf("Vaccination Zone %d is ready to vaccinate with %d slots​\n",zon->id,slots_num);
            i = 0;
            for(int j=1;j<=no_students;j++)     //changed
            {
                if(students[j].status ==1 && i<slots_num)
                {
                    blue(); printf("Student %d assigned a slot on the Vaccination Zone %d and waiting to be vaccinated\n",j,zon->id);
                    students[j].zone = zon->id;
                    students[j].status = 0;
                    students[j].vaccinationno++;
                    i++;
                }
                if(slots_num == i) break;
            }
            students_waiting = students_waiting - slots_num;
        }
        pthread_mutex_unlock(&mutex);
        i=0;
        for(int j=1;j<=no_students;j++)
        {
            if(students[j].zone == zon->id && i<slots_num)
            {
                students[j].ifvaccined = test(companies[zones[zon->id].whichcomp].prob);
                zon->capacity--;
                students[j].zone = 0;
                i++;
                students[j].antibodytest =1;
                if(students[j].ifvaccined==1)
                {
                    magenta(); printf("Student %d has tested 'positive' for antibodies.\n",j);
                    stud_rem--;
                    if(stud_rem==0){ red(); printf("Simulation over\n"); exit(0);}
                }
                else{
                    magenta(); printf("Student %d has tested 'negative' for antibodies.\n",j);
                    if(students[j].vaccinationno>=3)
                    {
                        stud_rem--;
                        if(stud_rem == 0){
                            red(); printf("Simulation over\n"); exit(0);
                        }
                    }
                }
            }
        }
        if(zon->capacity==0)
        {
            companies[zon->whichcomp].batchesUsed--;
            zon->status=0;
            yellow(); printf("Vaccination Zone %d has run out of Vaccines.\n",zon->id);
        } 
    }
} 
int main()
{
    srand(time(0));
    printf("Enter number of pharmaceutical companies: ");
    scanf("%d",&no_company);
    printf("Enter number of vaccination zones: ");
    scanf("%d",&no_zones);
    printf("Enter number of students: ");
    scanf("%d",&no_students);
    //take input for probability
    students_waiting =0;
    stud_rem  = no_students;
    pthread_mutex_init(&mutex,NULL);
    
    companies = (company *)malloc((no_company+1)*sizeof(company));
    zones = (zone *)malloc((no_zones+1)*sizeof(zone));
    students = (student *)malloc((no_students+1)*sizeof(student)); 

    printf("Enter probabilities for the companies: ");
    for(int i=1;i<=no_company;i++)
    {
        scanf("%f",&companies[i].prob);
        companies[i].id = i;
        companies[i].status = 0;
    }

    for(int i=1;i<=no_zones;i++)
    {
        zones[i].id = i;
        zones[i].capacity = 0;
        zones[i].status = 0;    // empty 
        pthread_mutex_init(&(zones[i].lock_zone),NULL);
    }
    for(int i=1;i<=no_students;i++)
    {
        students[i].id=i;
        students[i].status = 1;   //waiting for vaccine 
        students[i].vaccinationno = 1;
    }
    red(); printf("Beginning Simulation\n\n");

    for(int i=1;i<=no_company;i++)
    {
        // printf("Hello\n");
        // printf("%d\n",companies[i].id);
        pthread_create(&(companies[i].thread_id),NULL,prep_vaccine, &companies[i]);
    }
    for(int i=1;i<=no_students;i++)
    {
        pthread_create(&(students[i].thread_id),NULL,students_coming,&students[i]);
    }
    for(int i=1;i<=no_zones;i++)
    {
        pthread_create(&(zones[i].thread_id),NULL,vaccinating,&zones[i]);
    }
    for(int i=1;i<=no_company;i++)
        pthread_join(companies[i].thread_id,NULL);
    for(int i=1;i<=no_students;i++)
        pthread_join(students[i].thread_id,NULL);
    for(int i=1;i<=no_zones;i++)
        pthread_join(zones[i].thread_id,NULL);
    
    for(int i=1;i<=no_zones;i++)
    {
        pthread_mutex_destroy(&(zones[i].lock_zone));
    }
    pthread_mutex_destroy(&mutex);
    red(); printf("Simulation Over\n"); 
    reset();
    return 0;
}
