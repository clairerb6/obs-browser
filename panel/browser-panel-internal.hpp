#pragma once

#include <QTimer>
#include <QPointer>
#include <QImage>
#include <QSize>
#include <QKeyEvent>
#include <QInputMethodEvent>
#include "browser-panel.hpp"
#include "cef-headers.hpp"

#include <vector>
#include <mutex>
#include <atomic>
#include <functional>

struct PopupWhitelistInfo {
	std::string url;
	QPointer<QObject> obj;

	inline PopupWhitelistInfo(const std::string &url_, QObject *obj_) : url(url_), obj(obj_) {}
};

extern std::mutex popup_whitelist_mutex;
extern std::vector<PopupWhitelistInfo> popup_whitelist;
extern std::vector<PopupWhitelistInfo> forced_popups;

/* ------------------------------------------------------------------------- */

class QCefWidgetInternal : public QCefWidget {
	Q_OBJECT

public:
	QCefWidgetInternal(QWidget *parent, const std::string &url, CefRefPtr<CefRequestContext> rqc);
	~QCefWidgetInternal();

	CefRefPtr<CefBrowser> cefBrowser;
	std::string url;
	std::string script;
	CefRefPtr<CefRequestContext> rqc;
	QTimer timer;
#ifndef __APPLE__
	QPointer<QWindow> window;
	QPointer<QWidget> container;
#endif
	bool allowAllPopups_ = false;
	bool windowlessMode_ = false;
	std::mutex osrFrameMutex_;
	QImage osrFrame_;
	std::atomic<bool> browserClosing_{false};
	std::atomic<int> osrPixelWidth_{1};
	std::atomic<int> osrPixelHeight_{1};

	virtual void resizeEvent(QResizeEvent *event) override;
	virtual void showEvent(QShowEvent *event) override;
	virtual void hideEvent(QHideEvent *event) override;
	virtual void paintEvent(QPaintEvent *event) override;
	virtual QPaintEngine *paintEngine() const override;
	virtual void focusInEvent(QFocusEvent *event) override;
	virtual void focusOutEvent(QFocusEvent *event) override;
	virtual void keyPressEvent(QKeyEvent *event) override;
	virtual void keyReleaseEvent(QKeyEvent *event) override;
	virtual void inputMethodEvent(QInputMethodEvent *event) override;
	virtual void mouseMoveEvent(QMouseEvent *event) override;
	virtual void mousePressEvent(QMouseEvent *event) override;
	virtual void mouseReleaseEvent(QMouseEvent *event) override;
	virtual void wheelEvent(QWheelEvent *event) override;
	virtual void enterEvent(QEnterEvent *event) override;
	virtual void leaveEvent(QEvent *event) override;

	virtual void setURL(const std::string &url) override;
	virtual void setStartupScript(const std::string &script) override;
	virtual void allowAllPopups(bool allow) override;
	virtual void closeBrowser() override;
	virtual void reloadPage() override;
	virtual bool zoomPage(int direction) override;
	virtual void executeJavaScript(const std::string &script) override;
	bool isWindowlessMode() const { return windowlessMode_; }
	bool IsShuttingDown() const { return browserClosing_.load(std::memory_order_relaxed); }
	QSize GetOffscreenPixelSize() const
	{
		return QSize(osrPixelWidth_.load(std::memory_order_relaxed),
			     osrPixelHeight_.load(std::memory_order_relaxed));
	}
	void UpdateOffscreenFrame(const void *buffer, int width, int height);

	void finishCloseBrowser();
	void Resize();
	void UpdateOffscreenSizeCache();
	void PostHostEvent(const std::function<void(CefRefPtr<CefBrowserHost>)> &fn);
	void KickWindowlessRedrawOnce();
	void KickWindowlessRedrawBurst(int count = 4, int intervalMs = 120);

#ifdef __linux__
private:
	bool needsDeleteXdndProxy = true;
	void unsetToplevelXdndProxy();
#endif

public slots:
	void Init();

signals:
	void readyToClose();
};
