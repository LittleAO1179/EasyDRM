
#include <cstddef>
#include <qobject.h>
#include <memory>

class Model
{
public:
    Model();
    ~Model();

    // 单例模式
    Model(const Model&) = delete;
    Model& operator=(const Model&) = delete;

    // 全局访问点
    static Model& getInstance()
    {
        static Model instance;
        return instance;
    }

    void SetSavePath(const QString& path);
    const QString& GetSavePath() const;

    void SetChoosePath(const QString& path);
    const QString& GetChoosePath() const;

    void SetKey(unsigned char* key, size_t keySize);
    std::unique_ptr<unsigned char[]>& GetKey();
    const size_t& GetKeySize() const;

private:
    QString mSavePath;
    QString mChoosePath;

    std::unique_ptr<unsigned char[]> mKey;
    size_t mKeySize;
};