# BACK TO COLLEGE
---

## Overview
This program simulates the process fo production, distribution and testing of vaccines of students. Companied produces the vaccines which are then distributed to different vaccination zones and students are randomly allocated vaccination zones, they get vaccinated and then checked for antibody test. If they do not pas the antiobody test then thry are again sent for getting vaccination. This process repeats for three times, if they fails for the third time then they are sent home again.

## Implementation

- Threads are created for pharmaceutical companies, vaccination zones and students.
- three struct are created whicha are passes during the initialization of thread.

## code is divided into 4 parts
- prep_vaccines
- distribute them
- vaccination
- students_coming


#### prep vaccines
 - Each Pharmaceutical Company is treated as a separate thread. 
 - The Company takes a random amount of time to prepare the vaccines .
 - It produces r(also random) batches of vaccines.

#### distributing vaccines
 - It checks if any of the zones has ran out of vaccines by iterating over the struct array of the zones (i.e `zones[i].status`).
 -The mutex named ensures that no two companies provide vaccines to the same zone at the same time. 
 - After distributing all the vaccines to the zones , the company threads busy -waits for its vaccines to get exhausted on students in vaccination zones.
 - After the vaccines are exhausted the company again starts manufacturing vaccines by going back to the make vaccines function

#### Pharmaceutical companies
- The company produces random number of batches and also it takes random amount of time to prepare batches.
- Each batch has the capacity(random) to vaccinate the students.
- The mutex ensures that no two companies provide vaccines to same zone at the same time.
- After the vaccines are used up then company again starts manfacturing vaccines.

#### Vaccination Zones
- Initially in the main function zones[i].status struct array is all set to 0 and the zone thread busy waits for the companies to deliever the vaccines.
-  After the zones receive the vaccines ,it creates random slots. The value of random is set so that it does not exceed the students waiting and the vaccines remaining in the zones.
- If the zone ran out of vaccines it changes its signal array and thus informing companies that it needs vaccines and then waits for vaccines to arrive.
- The mutex ensures that the same student is not allocated to two different zones at the same time.

#### Students
- Students signals when they are ready for getting vaccination.
- The student waits(busy waiting) for the zone to get allocated.
- It the student test positive then he is sent to college else he has to join the queue.
- The number of times student comes for test is stored in students[].vaccinationno and has the limit 3.
- Vaccination not possible if either n or m or o is equal to zero.