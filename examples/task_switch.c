#include <uapi/linux/ptrace.h>
#include <linux/sched.h>

struct key_t {
  u32 prev_pid;
  u32 curr_pid;
};
// map_type, key_type, leaf_type, table_name, num_entry
BPF_TABLE("hash", struct key_t, u64, stats, 1024);
int count_sched(struct pt_regs *ctx) {
  struct key_t key = {};
  u64 zero = 0, *val;

  key.curr_pid = bpf_get_current_pid_tgid();
  bpf_probe_read(&key.prev_pid, 4,
		 (void *)ctx->di + offsetof(struct task_struct, pid));

  val = stats.lookup_or_init(&key, &zero);
  (*val)++;
  return 0;
}
