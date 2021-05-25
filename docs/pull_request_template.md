1. A brief but clear description of the issues/features

2. The brief description of the solution

3. The list of (newly) created test cases and test reports

4. Is necessary update is done on documents/reports?

5. Is user interface changed? Has the change updated in the history document? Has notice email sent to compiler team, CE/QA head and Peichen?

6. Is infrastructure APIs (codegen/program_analysis.c/h) changed? If so, did change history commented

7. Are other modules (owners) related to this PR?  Need to approve as well

8. Any other information/note needed in the code review and sign-off ?

9. Have you run "make check" before final push for coding style check?

-------------------------------------------------------
Python code check list|Dev(Y/N)|Rev1(Y/N)|Rev2(Y/N)|...
-------------------------------------------------------
1. Python lint 10 points------------------------|
2. Code coverage: 100%--------------------------|
3. ZeroDivisionError----------------------------|
4. OverflowError: exp usage---------------------|
5. KeyError-------------------------------------|
6. IndexError-----------------------------------|
7. ValueError: int(x)---------------------------|
8. StopIteration: iterator larger than loop tc--|
9. If logic can be replaced by built-in function|
10.If have global variable, remove it-----------|
11.If open file with "with open" API------------|
12.If duplicate logic can merge to one method---|
13.If enough comments for complicate code-------|
14.If enough info print to log for quick debug--|
15.If each method have clear input and output---|
16.If method and class argument can be fewer----|
17.If avoid mock in testing---------------------|
18.If have negative test cases------------------|
-------------------------------------------------------
