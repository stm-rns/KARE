# -*- mode: python ; coding: utf-8 -*-

block_cipher = None


a = Analysis(['bmp565_128x128_generator.py'],
             pathex=['C:\\git\\KARE\\05-Tools'],
             binaries=[],
             datas=[],
             hiddenimports=[],
             hookspath=[],
             runtime_hooks=[],
             #excludes=['tcl','tk','FixTk','_tkinter','Tkinter','numpy'],
             excludes=['numpy'],
             win_no_prefer_redirects=False,
             win_private_assemblies=False,
             cipher=block_cipher,
             noarchive=False)
pyz = PYZ(a.pure, a.zipped_data,
             cipher=block_cipher)
exe = EXE(pyz,
          a.scripts,
          a.binaries,
          a.zipfiles,
          a.datas,
          [],
          name='bmp565_128x128_generator',
          debug=False,
          bootloader_ignore_signals=False,
          strip=False,
          upx=False,
          upx_exclude=[],
          runtime_tmpdir=None,
          console=False )
