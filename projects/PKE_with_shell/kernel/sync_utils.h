#ifndef _SYNC_UTILS_H_
#define _SYNC_UTILS_H_

// 自旋锁的结构
typedef struct {
	volatile uint32_t lock;
} spinlock_t;

static inline void sync_barrier(volatile int *counter, int all)
{
	int local;

	asm volatile("amoadd.w %0, %2, (%1)\n"
		     : "=r"(local)
		     : "r"(counter), "r"(1)
		     : "memory");

	if (local + 1 < all) {
		do {
			asm volatile("lw %0, (%1)\n"
				     : "=r"(local)
				     : "r"(counter)
				     : "memory");
		} while (local < all);
	}
}

// 设置自旋锁
static inline void spin_lock(spinlock_t *lock)
{
	uint32_t value;
	// 使用内联汇编来执行 amoswap 指令
	asm volatile("1: "
		     "amoswap.w.aq %0, %1, %2 \n\t" // 尝试获取锁
		     "bnez %0, 1b" // 如果锁已经被占用，继续自旋等待
		     : "=&r"(value) // 输出值为 value
		     : "r"(1), "m"(lock->lock) // 输入值为 1 和 lock->lock
		     : "memory" // 告诉编译器内联汇编会修改内存
	);
}

// 释放自旋锁
static inline void spin_unlock(spinlock_t *lock)
{
	// 直接将锁的值设置为 0
	lock->lock = 0;
}

#endif