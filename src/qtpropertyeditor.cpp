#include "qtpropertyeditor.h"
#include "qtproperty.h"
#include "qtpropertybrowserutils.h"
#include "qxtcheckcombobox.h"
#include "qtattributename.h"

#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QComboBox>
#include <QAbstractItemView>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QToolButton>
#include <QFileDialog>

#include <limits>

QtPropertyEditor::QtPropertyEditor(QtProperty *property)
    : property_(property)
{
    connect(property, SIGNAL(signalValueChange(QtProperty*)), this, SLOT(onPropertyValueChange(QtProperty*)));
    connect(property, SIGNAL(destroyed(QObject*)), this, SLOT(onPropertyDestory(QObject*)));
}

QtPropertyEditor::~QtPropertyEditor()
{
}

void QtPropertyEditor::onPropertyDestory(QObject * /*object*/)
{
    property_ = NULL;
    delete this;
}

void QtPropertyEditor::slotEditorDestory(QObject * /*object*/)
{
    delete this;
}

/********************************************************************/
QtIntSpinBoxEditor::QtIntSpinBoxEditor(QtProperty *property)
    : QtPropertyEditor(property)
    , editor_(0)
{
    value_ = property_->getValue().toInt();
    connect(property, SIGNAL(signalAttributeChange(QtProperty*,QString)), this, SLOT(slotSetAttribute(QtProperty*,QString)));
}

QWidget* QtIntSpinBoxEditor::createEditor(QWidget *parent)
{
    if(editor_ == 0)
    {
        editor_ = new QSpinBox(parent);
        editor_->setKeyboardTracking(false);

        slotSetAttribute(property_, QtAttributeName::MinValue);
        slotSetAttribute(property_, QtAttributeName::MaxValue);

        editor_->setValue(value_);

        connect(editor_, SIGNAL(valueChanged(int)), this, SLOT(slotEditorValueChange(int)));
        connect(editor_, SIGNAL(destroyed(QObject*)), this, SLOT(slotEditorDestory(QObject*)));
    }
    return editor_;
}

void QtIntSpinBoxEditor::slotEditorValueChange(int value)
{
    if(value == value_)
    {
        return;
    }
    value_ = value;

    if(property_ != 0)
    {
        property_->setValue(value);
    }
}

void QtIntSpinBoxEditor::onPropertyValueChange(QtProperty* property)
{
    value_ = property->getValue().toInt();
    if(editor_ != 0)
    {
        editor_->blockSignals(true);
        editor_->setValue(value_);
        editor_->blockSignals(false);
    }
}

void QtIntSpinBoxEditor::slotSetAttribute(QtProperty *property, const QString &name)
{
    if(name == QtAttributeName::MinValue)
    {
        QVariant v = property->getAttribute(QtAttributeName::MinValue);
        int minValue = (v.type() == QVariant::Int) ? v.toInt() : std::numeric_limits<int>::min();
        editor_->setMinimum(minValue);
    }
    else if(name == QtAttributeName::MaxValue)
    {
        QVariant v = property_->getAttribute(QtAttributeName::MaxValue);
        int maxValue = (v.type() == QVariant::Int) ? v.toInt() : std::numeric_limits<int>::max();
        editor_->setMaximum(maxValue);
    }
}

/********************************************************************/
QtDoubleSpinBoxEditor::QtDoubleSpinBoxEditor(QtProperty *property)
    : QtPropertyEditor(property)
    , editor_(0)
{
    value_ = property_->getValue().toDouble();
    connect(property_, SIGNAL(signalAttributeChange(QtProperty*,QString)), this, SLOT(slotSetAttribute(QtProperty*,QString)));
}

QWidget* QtDoubleSpinBoxEditor::createEditor(QWidget *parent)
{
    if(editor_ == 0)
    {
        editor_ = new QDoubleSpinBox(parent);
        editor_->setKeyboardTracking(false);

        slotSetAttribute(property_, QtAttributeName::MinValue);
        slotSetAttribute(property_, QtAttributeName::MaxValue);
        slotSetAttribute(property_, QtAttributeName::Decimals);

        editor_->setValue(value_);

        connect(editor_, SIGNAL(valueChanged(double)), this, SLOT(slotEditorValueChange(double)));
        connect(editor_, SIGNAL(destroyed(QObject*)), this, SLOT(slotEditorDestory(QObject*)));
    }
    return editor_;
}

void QtDoubleSpinBoxEditor::slotEditorValueChange(double value)
{
    if(value == value_)
    {
        return;
    }
    value_ = value;

    if(property_ != 0)
    {
        property_->setValue(value);
    }
}

void QtDoubleSpinBoxEditor::onPropertyValueChange(QtProperty* property)
{
    value_ = property->getValue().toDouble();
    if(editor_ != 0)
    {
        editor_->blockSignals(true);
        editor_->setValue(value_);
        editor_->blockSignals(false);
    }
}

void QtDoubleSpinBoxEditor::slotSetAttribute(QtProperty *property, const QString &name)
{
    if(name == QtAttributeName::MinValue)
    {
        QVariant v = property->getAttribute(QtAttributeName::MinValue);
        int minValue = (v.type() == QVariant::Int) ? v.toInt() : std::numeric_limits<int>::min();
        editor_->setMinimum(minValue);
    }
    else if(name == QtAttributeName::MaxValue)
    {
        QVariant v = property->getAttribute(QtAttributeName::MaxValue);
        int maxValue = (v.type() == QVariant::Int) ? v.toInt() : std::numeric_limits<int>::max();
        editor_->setMaximum(maxValue);
    }
    else if(name == QtAttributeName::Decimals)
    {
        QVariant v = property->getAttribute(name);
        if(v.type() == QVariant::Int)
        {
            editor_->setDecimals(v.toInt());
        }
    }
}

/********************************************************************/
QtStringEditor::QtStringEditor(QtProperty *property)
    : QtPropertyEditor(property)
    , editor_(NULL)
{
    value_ = property->getValue().toString();
}

QWidget* QtStringEditor::createEditor(QWidget *parent)
{
    if(editor_ == NULL)
    {
        editor_ = new QLineEdit(parent);
        editor_->setText(value_);
        connect(editor_, SIGNAL(editingFinished()), this, SLOT(slotEditFinished()));
        connect(editor_, SIGNAL(destroyed(QObject*)), this, SLOT(slotEditorDestory(QObject*)));
    }
    return editor_;
}

void QtStringEditor::onPropertyValueChange(QtProperty *property)
{
    value_ = property->getValue().toString();
    if(editor_ != NULL)
    {
        editor_->blockSignals(true);
        editor_->setText(value_);
        editor_->blockSignals(false);
    }
}

void QtStringEditor::slotEditFinished()
{
    QString text = editor_->text();
    if(value_ != text)
    {
        value_ = text;
        if(property_ != 0)
        {
            property_->setValue(value_);
        }
    }
}

/********************************************************************/
QtEnumEditor::QtEnumEditor(QtProperty *property)
    : QtPropertyEditor(property)
    , editor_(NULL)
{
    value_ = property_->getValue().toInt();
    connect(property_, SIGNAL(signalAttributeChange(QtProperty*,QString)), this, SLOT(slotSetAttribute(QtProperty*,QString)));
}

QWidget* QtEnumEditor::createEditor(QWidget *parent)
{
    if(editor_ == NULL)
    {
        editor_ = new QComboBox(parent);
        editor_->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
        editor_->setMinimumContentsLength(1);
        editor_->view()->setTextElideMode(Qt::ElideRight);

        slotSetAttribute(property_, QtAttributeName::EnumName);

        editor_->setCurrentIndex(value_);

        connect(editor_, SIGNAL(currentIndexChanged(int)), this, SLOT(slotEditorValueChange(int)));
        connect(editor_, SIGNAL(destroyed(QObject*)), this, SLOT(slotEditorDestory(QObject*)));
    }
    return editor_;
}

void QtEnumEditor::onPropertyValueChange(QtProperty *property)
{
    value_ = property->getValue().toInt();
    if(editor_ != NULL)
    {
        editor_->blockSignals(true);
        editor_->setCurrentIndex(value_);
        editor_->blockSignals(false);
    }
}

void QtEnumEditor::slotEditorValueChange(int index)
{
    if(index != value_)
    {
        value_ = index;
        property_->setValue(value_);
    }
}

void QtEnumEditor::slotSetAttribute(QtProperty * property, const QString &name)
{
    if(name == QtAttributeName::EnumName)
    {
        editor_->clear();

        QStringList enumNames = property->getAttribute(QtAttributeName::EnumName).toStringList();
        editor_->addItems(enumNames);
    }
}

/********************************************************************/
QtFlagEditor::QtFlagEditor(QtProperty *property)
    : QtPropertyEditor(property)
    , editor_(NULL)
{
    value_ = property_->getValue().toInt();
    connect(property_, SIGNAL(signalAttributeChange(QtProperty*,QString)), this, SLOT(slotSetAttribute(QtProperty*,QString)));
}

QWidget* QtFlagEditor::createEditor(QWidget *parent)
{
    if(NULL == editor_)
    {
        editor_ = new QxtCheckComboBox(parent);
        editor_->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
        editor_->setMinimumContentsLength(1);
        editor_->view()->setTextElideMode(Qt::ElideRight);
        editor_->setSeparator("|");

        slotSetAttribute(property_, QtAttributeName::FlagName);
        setValueToEditor(value_);

        connect(editor_, SIGNAL(destroyed(QObject*)), this, SLOT(slotEditorDestory(QObject*)));
        connect(editor_, SIGNAL(checkedItemsChanged(QStringList)), this, SLOT(checkedItemsChanged(QStringList)));
    }
    return editor_;
}

void QtFlagEditor::setValueToEditor(int value)
{
    QIntList flagValues;
    for(int i = 0; i < flagNames_.size(); ++i)
    {
        if(value & (1 << i))
        {
            flagValues.push_back(i);
        }
    }
    editor_->blockSignals(true);
    editor_->setCheckedIndices(flagValues);
    editor_->blockSignals(false);
}

void QtFlagEditor::onPropertyValueChange(QtProperty *property)
{
    value_ = property->getValue().toInt();
    if(NULL != editor_)
    {
        setValueToEditor(value_);
    }
}

void QtFlagEditor::checkedItemsChanged(const QStringList& /*items*/)
{
    QIntList indices = editor_->checkedIndices();
    int value = 0;
    foreach(int index, indices)
    {
        value |= (1 << index);
    }
    if(value != value_)
    {
        value_ = value;
        property_->setValue(value_);
    }
}

void QtFlagEditor::slotSetAttribute(QtProperty * property, const QString &name)
{
    if(name == QtAttributeName::FlagName)
    {
        editor_->clear();

        flagNames_ = property->getAttribute(QtAttributeName::FlagName).toStringList();
        editor_->addItems(flagNames_);
    }
}

/********************************************************************/
QtBoolEditor::QtBoolEditor(QtProperty *property)
    : QtPropertyEditor(property)
    , editor_(NULL)
{
    value_ = property_->getValue().toBool();
}

QWidget* QtBoolEditor::createEditor(QWidget *parent)
{
    if(NULL == editor_)
    {
        editor_ = new QtBoolEdit(parent);
        editor_->setChecked(value_);

        connect(editor_, SIGNAL(toggled(bool)), this, SLOT(slotEditorValueChange(bool)));
        connect(editor_, SIGNAL(destroyed(QObject*)), this, SLOT(slotEditorDestory(QObject*)));
    }
    return editor_;
}

void QtBoolEditor::onPropertyValueChange(QtProperty * property)
{
    value_ = property->getValue().toBool();
    if(NULL != editor_)
    {
        editor_->blockSignals(true);
        editor_->setCheckState(value_ ? Qt::Checked : Qt::Unchecked);
        editor_->blockSignals(false);
    }
}

void QtBoolEditor::slotEditorValueChange(bool value)
{
    if(value != value_)
    {
        value_ = value;
        property_->setValue(value_);
    }
}

/********************************************************************/


QtColorEditor::QtColorEditor(QtProperty *property)
    : QtPropertyEditor(property)
    , editor_(NULL)
{
    value_ = QtPropertyBrowserUtils::variant2color(property->getValue());
}

QWidget* QtColorEditor::createEditor(QWidget *parent)
{
    if(NULL == editor_)
    {
        editor_ = new QtColorEditWidget(parent);
        editor_->setValue(value_);

        connect(editor_, SIGNAL(valueChanged(QColor)), this, SLOT(slotEditorValueChange(QColor)));
        connect(editor_, SIGNAL(destroyed(QObject*)), this, SLOT(slotEditorDestory(QObject*)));
    }
    return editor_;
}

void QtColorEditor::onPropertyValueChange(QtProperty * property)
{
    value_ = QtPropertyBrowserUtils::variant2color(property->getValue());
    if(NULL != editor_)
    {
        editor_->blockSignals(true);
        editor_->setValue(value_);
        editor_->blockSignals(false);
    }
}

void QtColorEditor::slotEditorValueChange(const QColor &color)
{
    if(color != value_)
    {
        value_ = color;
        property_->setValue(QtPropertyBrowserUtils::color2variant(value_));
    }
}


/********************************************************************/
QtFileEditor::QtFileEditor(QtProperty *property)
    : QtPropertyEditor(property)
    , editor_(NULL)
    , input_(NULL)
    , button_(NULL)
{
    value_ = property->getValue().toString();
}

QWidget* QtFileEditor::createEditor(QWidget *parent)
{
    if(NULL != editor_)
    {
        return editor_;
    }

    editor_ = new QWidget(parent);

    QHBoxLayout *layout = new QHBoxLayout(editor_);
    QtPropertyBrowserUtils::setupTreeViewEditorMargin(layout);
    layout->setSpacing(0);

    input_ = new QLineEdit();
    input_->setText(value_);
    connect(input_, SIGNAL(editingFinished()), this, SLOT(slotEditingFinished()));
    layout->addWidget(input_);

    button_ = new QToolButton();
    button_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Ignored);
    button_->setFixedWidth(20);
    button_->setText(tr("..."));
    //button_->installEventFilter(this);
    editor_->setFocusProxy(button_);
    editor_->setFocusPolicy(button_->focusPolicy());
    connect(button_, SIGNAL(clicked()), this, SLOT(slotButtonClicked()));
    layout->addWidget(button_);

    connect(editor_, SIGNAL(destroyed(QObject*)), this, SLOT(slotEditorDestory(QObject*)));
    return editor_;
}

void QtFileEditor::onPropertyValueChange(QtProperty *property)
{
    QString value = property->getValue().toString();
    if(editor_ != NULL)
    {
        setValue(value);
    }
}

void QtFileEditor::setValue(const QString &value)
{
    value_ = value;

    input_->blockSignals(true);
    input_->setText(value);
    input_->blockSignals(false);
}

void QtFileEditor::slotButtonClicked()
{
    QString path = QFileDialog::getOpenFileName();
    if(!path.isEmpty() && path != value_)
    {
        setValue(path);
        property_->setValue(path);
    }
}

void QtFileEditor::slotEditingFinished()
{
    QString text = input_->text();
    if(text != value_)
    {
        value_ = text;
        property_->setValue(value_);
    }
}
