using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Threading;

namespace RWMeshGen
{
    partial class ThreadSafeStatusWindow : Form
    {
        public ThreadSafeStatusWindow()
        {
            InitializeComponent();
        }
    }

    public class StatusWindow:IDisposable
    {
        private Thread thread;
        private string caption;
        private string status;
        private bool cancel;

        public StatusWindow(string caption, string status)
        {
            this.caption = caption;
            this.status = status;
            this.cancel = false;
            this.thread = null;

#if DEBUG
            return;
#endif

            this.thread = new Thread(new ParameterizedThreadStart(StatusWindowThread));
            this.thread.Start(this);
        }


        public void SetStatus(string str)
        {
            lock(this)
                this.status = str;
        }

        public void Close()
        {
#if DEBUG
            return;
#endif
            lock (this)
                this.cancel = true;
            this.thread.Join();
        }


        private static void StatusWindowThread(object obj)
        {
            try
            {
                StatusWindow _this = obj as StatusWindow;

                using (ThreadSafeStatusWindow wnd = new ThreadSafeStatusWindow())
                {
                    wnd.Show();

                    while (!_this.cancel)
                    {
                        lock (_this)
                        {
                            wnd.Text = _this.caption;
                            wnd.lblStatus.Text = _this.status;
                        }

                        Thread.Sleep(30);
                        Application.DoEvents();
                    }

                }
            }
            catch (ThreadAbortException e)
            {
            }
        }


        #region IDisposable Members

        public void Dispose()
        {
            this.Close();
        }

        #endregion
    }
}
