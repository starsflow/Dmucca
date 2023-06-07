/*
 * @Author: lxc
 * @Date: 05/25/2023
 */

struct Operation {
    bool is_read; //true: read operation; false: write operation
    void* key;
    void* value;
};
