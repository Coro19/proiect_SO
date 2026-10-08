# City Reports Manager

A city-infrastructure reporting system I built in C for my Operating Systems course at UPT. Inspectors file reports about problems in a district (potholes, broken lighting, flooding), and managers review and clean them up.

The point of the project was to work directly with the Linux system-call API: files, permissions, processes, signals and pipes, with no libraries on top. I built it over three phases during the semester.

## The programs

| Program | What it does | What it uses |
|---|---|---|
| `city_manager` | Command-line tool to add, list, view, filter and remove reports | `open`/`read`/`write`/`lseek`/`ftruncate` on a binary file of fixed-size records, `stat`/`chmod` permission checks, symlinks, `fork` + `execl` + `waitpid` |
| `monitor_reports` | Background process that is notified each time a report is added | `sigaction`, `SIGUSR1`/`SIGINT`, a PID file |
| `city_hub` | Interactive shell that starts the monitor and computes workload scores | `fork`, `exec`, `pipe`, `dup2` |
| `scorer` | Sums report severity per inspector for one district | Reads the binary records |

**How they connect:** `city_hub` starts the monitor in the background and relays its output through a pipe. When `city_manager` adds a report, it sends `SIGUSR1` to the monitor. `calculate_scores` forks one `scorer` per district, so all districts are scored in parallel, and collects the results through pipes.

## How data is stored

Each district is a folder:

| File | Contents | Permissions |
|---|---|---|
| `reports.dat` | Binary `Report` structs, appended one after another | `664` |
| `district.cfg` | Severity threshold | `640` |
| `logged_district` | Log of every action (timestamp, user, role, action) | `644` |
| `active_reports-<district>` | Symlink to `reports.dat`, created next to the folder | – |

The two roles map onto Unix permission bits: **managers** are checked against the owner bits and **inspectors** against the group bits. Only managers can remove reports or districts and change the threshold.

## Build and run

```bash
make
```

**city_manager**

```bash
./city_manager --role inspector --user ana --add Fabric    # prompts for the report fields
./city_manager --role manager   --user ion --list Fabric
./city_manager --role manager   --user ion --view Fabric 1
./city_manager --role inspector --user ana --filter Fabric "severity:>=:2" "category:==:road"
./city_manager --role manager   --user ion --remove_report Fabric 1
./city_manager --role manager   --user ion --update_threshold Fabric 2
./city_manager --role manager   --user ion --remove_district Fabric
```

Filter conditions use the form `field:op:value`. The supported fields are `severity`, `category`, `inspector` and `timestamp`. Put the conditions in quotes, because otherwise the shell reads `>` as a redirection.

**city_hub**

```
$ ./city_hub
city_hub> start_monitor
city_hub> calculate_scores Fabric Centru
=== Workload Report ===
District: Fabric
  Inspector: ana                  Score: 3
  Inspector: ion                  Score: 1

=== End of Report ===
city_hub> exit
```

## AI usage

The course asked us to document any AI use. Mine is described in [AI_usage-ALL-phases.md](AI_usage-ALL-phases.md).
