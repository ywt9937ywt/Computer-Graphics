#include "OBJ_Loader.h"

OBJ_Loader::OBJ_Loader()
{

}

bool OBJ_Loader::load(QString fileName, QVector<Triangle>& tri_pool)
{
    if (fileName.mid(fileName.lastIndexOf('.')) != ".obj") {
        qDebug() << "file is not a obj file";
        return false;
    }

    QFile objFile(fileName);
    if (!objFile.open(QIODevice::ReadOnly)) {
        qDebug() << "open" << fileName << "failed";
        return false;
    }

    QVector<float> vertextPoints, texturePoints, normalPoints;
    QVector<std::tuple<int, int, int>> facesIndexs;
    while (!objFile.atEnd()) {
        QByteArray lineData = objFile.readLine();
        lineData = lineData.remove(lineData.count() - 1, 1);

        QList<QByteArray> strValues = lineData.split(' ');
        QString dataType = strValues.takeFirst();
        if (dataType == "v") {
            std::transform(strValues.begin(), strValues.end(), std::back_inserter(vertextPoints), [](QByteArray& str) {
                return str.toFloat();
            });
        }
        else if (dataType == "vt") {
            std::transform(strValues.begin(), strValues.end(), std::back_inserter(texturePoints), [](QByteArray& str) {
                return str.toFloat();
            });
        }
        else if (dataType == "vn") {
            std::transform(strValues.begin(), strValues.end(), std::back_inserter(normalPoints), [](QByteArray& str) {
                return str.toFloat();
            });
        }
        else if (dataType == "f") {
            std::transform(strValues.begin(), strValues.end(), std::back_inserter(facesIndexs), [](QByteArray& str) {
                QList<QByteArray> intStr = str.split('/');
                return std::make_tuple(intStr.first().toInt(), intStr.at(1).toInt(), intStr.last().toInt());
                //return str.toInt();
            });
        }
    }

    qDebug() << "facesIndexs:     " << facesIndexs.count();
    //for (auto& verFaceInfo : facesIndexs) {
    for (int i = 0; i < facesIndexs.count();i+=3) {
        Triangle *t = new Triangle();
        int ind_v1, ind_v2, ind_v3;
        ind_v1 = std::get<0>(facesIndexs.at(i));
        ind_v2 = std::get<0>(facesIndexs.at(i+1));
        ind_v3 = std::get<0>(facesIndexs.at(i+2));
        t->setVertex(0, QVector3D(vertextPoints.at(ind_v1 * 3 - 3), vertextPoints.at(ind_v1 * 3 - 2), vertextPoints.at(ind_v1 * 3 - 1)));
        t->setVertex(1, QVector3D(vertextPoints.at(ind_v2 * 3 - 3), vertextPoints.at(ind_v2 * 3 - 2), vertextPoints.at(ind_v2 * 3 - 1)));
        t->setVertex(2, QVector3D(vertextPoints.at(ind_v3 * 3 - 3), vertextPoints.at(ind_v3 * 3 - 2), vertextPoints.at(ind_v3 * 3 - 1)));
        
        int ind_vt1, ind_vt2, ind_vt3;
        ind_vt1 = std::get<1>(facesIndexs.at(i));
        ind_vt2 = std::get<1>(facesIndexs.at(i + 1));
        ind_vt3 = std::get<1>(facesIndexs.at(i + 2));
        t->setTexCoord(0, texturePoints.at(ind_vt1 * 2 - 2), texturePoints.at(ind_vt1 * 2 - 1));
        t->setTexCoord(1, texturePoints.at(ind_vt2 * 2 - 2), texturePoints.at(ind_vt2 * 2 - 1));
        t->setTexCoord(2, texturePoints.at(ind_vt3 * 2 - 2), texturePoints.at(ind_vt3 * 2 - 1));

        int ind_vn1, ind_vn2, ind_vn3;
        ind_vn1 = std::get<2>(facesIndexs.at(i));
        ind_vn2 = std::get<2>(facesIndexs.at(i + 1));
        ind_vn3 = std::get<2>(facesIndexs.at(i + 2));
        t->setNormal(0, QVector3D(normalPoints.at(ind_vn1 * 3 - 3), normalPoints.at(ind_vn1 * 3 - 2), normalPoints.at(ind_vn1 * 3 - 1)));
        t->setNormal(1, QVector3D(normalPoints.at(ind_vn2 * 3 - 3), normalPoints.at(ind_vn2 * 3 - 2), normalPoints.at(ind_vn2 * 3 - 1)));
        t->setNormal(2, QVector3D(normalPoints.at(ind_vn3 * 3 - 3), normalPoints.at(ind_vn3 * 3 - 2), normalPoints.at(ind_vn3 * 3 - 1)));
        
        tri_pool.push_back(*t);
        delete(t);
        /*int vIndex = std::get<0>(verFaceInfo);
        
        int tIndex = std::get<1>(verFaceInfo);
        int nIndex = std::get<2>(verFaceInfo);
        int vPointSizes = vertextPoints.count() / 3;
        int tPointSizes = texturePoints.count() / 2;
        int nPointSizes = normalPoints.count() / 3;
        vPoints << vertextPoints.at((vPointSizes + vIndex) * 3);
        vPoints << vertextPoints.at((vPointSizes + vIndex) * 3 + 1);
        vPoints << vertextPoints.at((vPointSizes + vIndex) * 3 + 2);

        tPoints << texturePoints.at((tPointSizes + tIndex) * 2);
        tPoints << texturePoints.at((tPointSizes + tIndex) * 2 + 1);

        nPoints << normalPoints.at((nPointSizes + nIndex) * 3);
        nPoints << normalPoints.at((nPointSizes + nIndex) * 3 + 1);
        nPoints << normalPoints.at((nPointSizes + nIndex) * 3 + 2);*/
    }
    vertextPoints.clear();
    texturePoints.clear();
    normalPoints.clear();
    facesIndexs.clear();
    
    objFile.close();
}