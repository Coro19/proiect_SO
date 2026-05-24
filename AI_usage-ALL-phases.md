#tools used

For this project I used Claude.

#promts given & what was generated

I described my Report struct and asked it to generate the function parse_condition(const char *input, char *field, char *op, char *value) which has the role of splitting field:op:value into three separate components.

Then I asked for the function match_condition(Report *r, const char *field, const char *op, const char *value) which returns 1 if it finds a match.

#what I changed

I extended cmd_filter to support multiple conditions, added a conditions array and a loop.

While Claude got the general logic right, it had some vulnerabilities such as input in parse_condition >64 would cause failure. It also did not check for invalid input in match_condition. char-to-int conversion was used without validation.

#what I learned

Claude can give a general solution to my problem, but lacks certain edge-cases. It can speed up what I do, but I always need to double-check its output.

#Phase 2

#prompts given & what was generated

I asked Claude for help with the remove_district function for the child process and I learned to use _exit.
I also asked Claude for help regarding struct sigaction and it suggested using gnu11.

It generated the fork-exec-waitpid structure along with the explanation of exit() and _exit() difference.

#what I changed

I added log_action() before the fork in remove_district.

#what I learned

_exit() must be used in child processes to avoid flushing the parent's stdio buffers.

#Phase 3

#prompts given & what was generated

I asked Claude to explain why fflush(stdout) is necessary.