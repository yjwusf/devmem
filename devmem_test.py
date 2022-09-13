import subprocess

cacheable = 0x040000000
uncacheable = 0x440000000

def write_c_read_unc(word, addr):
    cmd1 = ['sudo', 'busybox', 'devmem', hex(addr + cacheable), 'w', word]
    res1 = subprocess.check_output(cmd1) 
    cmd2 = ['sudo', 'busybox', 'devmem', hex(addr + uncacheable) ]
    res2 = subprocess.check_output(cmd2) 
    print("addresses are ", hex(addr + cacheable), hex(addr + uncacheable))
    print("and results are ", res1.strip(), res2.strip())
    if res2 == word:
        print("Write cache read uncache passes!")

def write_unc_read_c(word, addr):
    cmd1 = ['sudo', 'busybox', 'devmem', hex(addr + uncacheable), 'w', word]
    res1 = subprocess.check_output(cmd1) 
    cmd2 = ['sudo', 'busybox', 'devmem', hex(addr + cacheable) ]
    res2 = subprocess.check_output(cmd2) 
    print("addresses are ", hex(addr + cacheable), hex(addr + uncacheable))
    print("and results are ", res1, res2)
    if res2 == word:
        print("Write uncache read cache passes!")

def write_uc_read_uc(word, addr):
    cmd1 = ['sudo', 'busybox', 'devmem', hex(addr + uncacheable), 'w', word]
    res1 = subprocess.check_output(cmd1) 
    cmd2 = ['sudo', 'busybox', 'devmem', hex(addr + uncacheable) ]
    res2 = subprocess.check_output(cmd2) 
    print("addresses are ", hex(addr + cacheable), hex(addr + uncacheable))
    print("and results are ", res1, res2)
    if res2 == word:
        print("Write uncache read uncache passes!")

if __name__ == "__main__":
    x = 0x200000
    word = '0xD33DBEEF'
    write_c_read_unc(word, x)
    word = '0xD3ADBEEF'
    write_unc_read_c(word, x)
