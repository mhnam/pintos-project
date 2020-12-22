#include <hash.h>
#include <string.h>
#include "threads/malloc.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "threads/interrupt.h"
#include "userprog/pagedir.h"
#include "vm/page.h"
#include "vm/frame.h"
#include "vm/swap.h"
#include <string.h>

void vm_init (struct hash* vm)
{
/* hash_init()으로 해시테이블 초기화 */
/* 인자로 해시 테이블과 vm_hash_func과 vm_less_func 사용 */
	ASSERT(vm != NULL);
	hash_init (vm, vm_hash_func, vm_less_func, NULL);
}

static unsigned vm_hash_func (const struct hash_elem *e,void *aux)
{
/* hash_entry()로 element에 대한 vm_entry 구조체 검색 */
/* hash_int()를 이용해서 vm_entry의 멤버 vaddr에 대한 해시값을
구하고 반환 */
	ASSERT(e != NULL);
	struct vm_entry* item = hash_entry(e, struct vm_entry, elem);
	return hash_int(item->vaddr);
}

static bool vm_less_func (const struct hash_elem *a, const struct
hash_elem *b)
{
/* hash_entry()로 각각의 element에 대한 vm_entry 구조체를 얻은
후 vaddr 비교 (b가 크다면 TRUE, a가 크다면 false */
	ASSERT(a != NULL);
	ASSERT(b != NULL);
	struct vm_entry* tmp_hashitem1 = hash_entry(a, struct vm_entry, elem);
	struct vm_entry* tmp_hashitem2 = hash_entry(b, struct vm_entry, elem);
	if(tmp_hashitem1->vaddr < tmp_hashitem2->vaddr)
		return true;
	else if(tmp_hashitem1->vaddr >= tmp_hashitem2->vaddr)
		return false;
}

bool insert_vme (struct hash *vm, struct vm_entry *vme)
{
  ASSERT(vm != NULL);
  ASSERT(vme != NULL);
  ASSERT(pg_ofs (vme->vaddr) == 0);
  return hash_insert (vm, &vme->elem) == NULL;
}

bool delete_vme (struct hash *vm, struct vm_entry *vme)
{
  ASSERT(vm != NULL);
  ASSERT(vme != NULL);
	
  if (hash_delete(vm, &vme->elem) == 0)
    return false;
  free(vme);
  return true;
}

struct vm_entry *find_vme (void *vaddr)
{
/* pg_round_down()으로 vaddr의 페이지 번호를 얻음 */
/* hash_find() 함수를 사용해서 hash_elem 구조체 얻음 */
/* 만약 존재하지 않는다면 NULL 리턴 */
/* hash_entry()로 해당 hash_elem의 vm_entry 구조체 리턴 */

  struct hash *vm = &thread_current()->vm;
  struct vm_entry vme;
  struct hash_elem *e;

  vme.vaddr = pg_round_down(vaddr);
	ASSERT(pg_ofs(vme.vaddr) == 0);
  e = hash_find (vm, &vme.elem);
  return e != NULL ? hash_entry (e, struct vm_entry, elem) : NULL;
}

void vm_destroy (struct hash *vm)
{
/* hash_destroy()으로 해시테이블의 버킷리스트와 vm_entry들을 제거 */
  ASSERT (vm != NULL);
  hash_destroy (vm, vm_destroy_func);
}

static void
vm_destroy_func (struct hash_elem *e, void *aux UNUSED)
{
  ASSERT (e != NULL);
  free(hash_entry(e, struct vm_entry, elem));
}

bool load_file (void* kaddr, struct vm_entry *vme)
{
  ASSERT (kaddr != NULL);
  ASSERT (vme != NULL);
  ASSERT (vme->type == VM_BIN);

  if (file_read_at (vme->file, kaddr, vme->read_bytes, vme->offset) != (int) vme->read_bytes)
      return false;
	
  memset (kaddr + vme->read_bytes, 0, vme->zero_bytes);
  return true;
}