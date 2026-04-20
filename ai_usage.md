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