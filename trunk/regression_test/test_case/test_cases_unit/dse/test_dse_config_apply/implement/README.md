[Bug 1887]
This folder "implement" is a workaround solution for frontend_pass.pl that checks
if this folder exists or not to determine the use of single_flow.
It means that without this folder the frontend_pass.pl will use single_flow and
enforce to run "final_code_gen" pass instead of any others.
