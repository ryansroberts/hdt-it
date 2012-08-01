#include "stringutils.hpp"
#include "regexmodel.hpp"

#include <QFont>

class QFont;

#include "../hdt-lib/src/dictionary/PFCPlusDictionary.hpp"

RegexModel::RegexModel(HDTManager *manager) : hdtManager(manager), numResults(0), results(NULL)
{
}

RegexModel::~RegexModel()
{
    if(results!=NULL) {
        free(results);
    }
}

void RegexModel::setQuery(QString query)
{
    if(hdtManager->hasHDT()) {
#if 1
        hdt::PFCPlusDictionary &dict = dynamic_cast<hdt::PFCPlusDictionary &>(hdtManager->getHDT()->getDictionary());
        QByteArray arr = query.toUtf8();
        numResults = dict.substringToId((uchar *)arr.data(), arr.size(), &results);
#else
        numResults = hdtManager->getHDT()->getDictionary().getNobjects();
        results = (ulong *) malloc(numResults*sizeof(ulong));

        for(int i=0;i<numResults;i++) {
            results[i]=i+1;
        }
#endif
        emit layoutChanged();
    }
}

int RegexModel::rowCount(const QModelIndex &parent) const
{
    return (int)numResults;
}

int RegexModel::columnCount(const QModelIndex &parent) const
{
    return 2;
}

QVariant RegexModel::data(const QModelIndex &index, int role) const
{
    if(results==NULL) {
        return QVariant();
    }

    switch(role) {
    case Qt::ToolTipRole:
    case Qt::DisplayRole:
    {
        hdt::Dictionary &d = hdtManager->getHDT()->getDictionary();

        try {
            switch(index.column()) {
            case 0:
            {
                hdt::Triples &t = hdtManager->getHDT()->getTriples();

                hdt::TripleID tripleObject(0, 0, results[index.row()]);
                hdt::IteratorTripleID *it = t.search(tripleObject);

                int nResults = it->estimatedNumResults();
                delete it;

                return nResults;
            }
            case 1: {
                return stringutils::cleanN3String(d.idToString(results[index.row()], hdt::OBJECT).c_str());
            }
            }

        } catch (char *e) {
            cout << "Error accesing dictionary: " << e << endl;
        } catch (const char *e) {
            cout << "Error accesing dictionary: " << e << endl;
        }
        return QVariant();

        break;
    }
    case Qt::FontRole:
    {
        static QFont font;
        font.setPointSize(10);
        return font;
    }

    }
    return QVariant();
}

QVariant RegexModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch(role) {
    case Qt::DisplayRole:
    {
        if(orientation == Qt::Horizontal) {
            switch(section) {
            case 0:
                return tr("Triples  ");
            case 1:
                return tr("Object Literal");
            }
        } else {
            return QString::number(results[section]);
        }
        break;
    }
    }

    return QVariant();
}

void RegexModel::updateDatasetChanged()
{
    if(results==NULL) {
        free(results);
    }
    numResults = 0;
    results=NULL;

    emit layoutChanged();
}
