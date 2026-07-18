import tkinter as tk
from tkinter import ttk, filedialog, messagebox
import subprocess
import os
import csv
import io

class TelemetryViewer(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title("assetto corsa telemetry viewer")
        self.geometry("1100x700")
        
        self.tools_dir = os.path.dirname(__file__)
        self.dumper_exe = os.path.join(self.tools_dir, "dumper.exe")
        self.cpp_file = os.path.join(self.tools_dir, "dumper.cpp")
        
        self.full_data = [] # stores all rows
        self.current_file = None
        self.sort_col = None
        self.sort_reverse = False
        
        self.compile_dumper()
        self.create_widgets()
        
    def compile_dumper(self):
        if not os.path.exists(self.dumper_exe):
            try:
                subprocess.run(["g++", self.cpp_file, "-o", self.dumper_exe, "-O2", "-static"], check=True)
            except Exception as e:
                messagebox.showerror("compiler error", f"failed to compile dumper.cpp:\n{e}")
            
    def create_widgets(self):
        top_frame = tk.Frame(self)
        top_frame.pack(side=tk.TOP, fill=tk.X, padx=10, pady=10)
        
        # track length
        tk.Label(top_frame, text="track len (m):").pack(side=tk.LEFT)
        self.track_length_var = tk.StringVar(value="5000.0")
        track_entry = tk.Entry(top_frame, textvariable=self.track_length_var, width=10)
        track_entry.pack(side=tk.LEFT, padx=5)
        track_entry.bind("<Return>", lambda e: self.recalc_meters())
        
        # lap dropdown
        tk.Label(top_frame, text="lap:").pack(side=tk.LEFT, padx=(10,0))
        self.lap_var = tk.StringVar(value="all")
        self.lap_combo = ttk.Combobox(top_frame, textvariable=self.lap_var, width=5, state="readonly")
        self.lap_combo.pack(side=tk.LEFT, padx=5)
        self.lap_combo.bind("<<ComboboxSelected>>", lambda e: self.apply_filters())
        
        # global search
        tk.Label(top_frame, text="search:").pack(side=tk.LEFT, padx=(15,0))
        self.search_var = tk.StringVar()
        search_entry = tk.Entry(top_frame, textvariable=self.search_var, width=20)
        search_entry.pack(side=tk.LEFT, padx=5)
        self.search_var.trace_add("write", lambda *args: self.apply_filters())
        
        # load btn
        tk.Button(top_frame, text="open file", command=self.load_file, bg="#4CAF50", fg="white").pack(side=tk.LEFT, padx=15)
        
        self.status_var = tk.StringVar(value="ready.")
        tk.Label(top_frame, textvariable=self.status_var, fg="gray").pack(side=tk.RIGHT, padx=10)

        # treeview
        main_frame = tk.Frame(self)
        main_frame.pack(side=tk.BOTTOM, fill=tk.BOTH, expand=True, padx=10, pady=(0,10))
        
        self.columns = ("Lap", "Index", "Meters", "NormPos", "SpeedKmh", "Gear", "RPM", "Gas", "Brake", "Steer", "LapTime_ms", "Sector")
        self.tree = ttk.Treeview(main_frame, columns=self.columns, show="headings")
        
        widths = {
            "Lap": 40, "Index": 60, "Meters": 80, "NormPos": 80, "SpeedKmh": 80, 
            "Gear": 60, "RPM": 60, "Gas": 60, "Brake": 60, 
            "Steer": 80, "LapTime_ms": 100, "Sector": 60
        }
        
        for col in self.columns:
            self.tree.heading(col, text=col.lower(), command=lambda c=col: self.sort_by(c))
            self.tree.column(col, width=widths.get(col, 80), anchor=tk.CENTER)
            
        vsb = ttk.Scrollbar(main_frame, orient=tk.VERTICAL, command=self.tree.yview)
        hsb = ttk.Scrollbar(main_frame, orient=tk.HORIZONTAL, command=self.tree.xview)
        self.tree.configure(yscrollcommand=vsb.set, xscrollcommand=hsb.set)
        
        vsb.pack(side=tk.RIGHT, fill=tk.Y)
        hsb.pack(side=tk.BOTTOM, fill=tk.X)
        self.tree.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        
    def sort_by(self, col):
        if self.sort_col == col:
            self.sort_reverse = not self.sort_reverse
        else:
            self.sort_reverse = False
            self.sort_col = col
            
        self.apply_filters()

    def load_file(self):
        file_path = filedialog.askopenfilename(filetypes=[("ac telemetry", "*.actl"), ("all files", "*.*")])
        if not file_path: return
        self.current_file = file_path
        self.process_data()
            
    def recalc_meters(self):
        if not self.full_data: return
        try:
            length = float(self.track_length_var.get())
        except ValueError:
            return
            
        lap_idx = self.columns.index("Lap")
        norm_idx = self.columns.index("NormPos")
        meter_idx = self.columns.index("Meters")
        
        for row in self.full_data:
            try:
                norm_pos = float(row[norm_idx])
                row[meter_idx] = f"{norm_pos * length:.2f}"
            except ValueError:
                pass
                
        self.apply_filters()

    def process_data(self):
        try:
            length = float(self.track_length_var.get())
        except ValueError:
            length = 5000.0
            
        self.status_var.set("loading...")
        self.update_idletasks()
        
        if not os.path.exists(self.dumper_exe):
            self.compile_dumper()
            
        try:
            res = subprocess.run([self.dumper_exe, self.current_file, str(length)], capture_output=True, text=True)
            if res.returncode != 0:
                messagebox.showerror("dumper error", res.stderr)
                return
                
            reader = csv.reader(io.StringIO(res.stdout))
            next(reader, None) # skip header
            
            self.full_data = [row for row in reader if row]
            
            # populate lap dropdown
            lap_idx = self.columns.index("Lap")
            laps = sorted(list(set(int(row[lap_idx]) for row in self.full_data if row[lap_idx].isdigit())))
            self.lap_combo["values"] = ["all"] + [str(l) for l in laps]
            self.lap_combo.current(0)
            
            self.apply_filters()
        except Exception as e:
            messagebox.showerror("error", str(e))
            
    def apply_filters(self):
        if not hasattr(self, 'tree'): return 
        
        self.tree.delete(*self.tree.get_children())
        if not self.full_data: return
        
        lap_filter = self.lap_var.get()
        lap_idx = self.columns.index("Lap")
        
        search_term = self.search_var.get().strip().lower()
        
        filtered = []
        for row in self.full_data:
            # lap filter
            if lap_filter != "all" and row[lap_idx] != lap_filter:
                continue
            
            # global text search
            if search_term:
                match = any(search_term in str(cell).lower() for cell in row)
                if not match:
                    continue
                    
            filtered.append(row)
                
        # sorting
        if self.sort_col:
            idx = self.columns.index(self.sort_col)
            try:
                filtered.sort(key=lambda r: float(r[idx]), reverse=self.sort_reverse)
            except ValueError:
                filtered.sort(key=lambda r: r[idx].lower(), reverse=self.sort_reverse)
                
        for row in filtered:
            self.tree.insert("", tk.END, values=row)
            
        self.status_var.set(f"showing {len(filtered)} items.")

if __name__ == "__main__":
    app = TelemetryViewer()
    app.mainloop()
