
#include <qobject.h>

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

private:
    QString mSavePath;
    QString mChoosePath;
};