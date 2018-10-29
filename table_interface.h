#ifndef TABLE_INTERFACE
#define TABLE_INTERFACE


namespace swarm
{

class table_interface
{
public:
    virtual std::shared_ptr<const answer> get(OID index) = 0;
    virtual std::shared_ptr<const object> get_object(OID index) = 0;
    virtual std::shared_ptr<const answer> get_with_limit(size_t start, size_t limit, std::function<bool(const object&)> where) = 0;
    virtual std::shared_ptr<const answer> find(std::shared_ptr<const key_interface>& key_ptr, size_t start, size_t limit, std::function<bool(const object&)> where) = 0;
    virtual std::shared_ptr<const answer> find_range(std::shared_ptr<const key_interface>& lower_ptr, std::shared_ptr<const key_interface>& upper_ptr, size_t start, size_t limit, std::function<bool(const object&)> where) = 0;
    virtual OID insert(std::shared_ptr<const object> object_ptr) = 0;
    virtual bool update(OID index, std::shared_ptr<const object> object_ptr) = 0;
    virtual bool remove(OID index) = 0;
    virtual size_t size() = 0;
    virtual bool get_unique_keys_flag() = 0;

    virtual bool try_lock_record(OID oid) = 0;
    virtual bool try_lock_table() = 0;
    virtual bool unlock_record(OID oid) = 0;
    virtual bool unlock_table() = 0;

    virtual ~table_interface();
};

} // end namespace swarm

#endif
