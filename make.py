#!/usr/bin/python

import os, sys, multiprocessing
from subprocess import Popen,PIPE

# Definitions

INCLUDES = [
os.path.join('BulletSoftBody', 'btSoftBody.h'),
os.path.join('BulletSoftBody', 'btSoftRigidDynamicsWorld.h'), 
os.path.join('BulletSoftBody', 'btDefaultSoftBodySolver.h'),
os.path.join('BulletSoftBody', 'btSoftBodyRigidBodyCollisionConfiguration.h'),
os.path.join('BulletSoftBody', 'btSoftBodyHelpers.h'),

'btBulletDynamicsCommon.h',
os.path.join('BulletDynamics', 'Character', 'btKinematicCharacterController.h'),

os.path.join('BulletCollision', 'CollisionShapes', 'btHeightfieldTerrainShape.h'),
os.path.join('BulletCollision', 'CollisionShapes', 'btConvexPolyhedron.h'),
os.path.join('BulletCollision', 'CollisionShapes', 'btShapeHull.h'),
# os.path.join('BulletCollision', 'CollisionShapes', 'btConvexInternalShape.h'),
# os.path.join('BulletCollision', 'CollisionShapes', 'btPolyhedralConvexShape.h'),
# os.path.join('BulletCollision', 'CollisionShapes', 'btTriangleCallback.h'),
# os.path.join('BulletCollision', 'CollisionShapes', 'btConvexShape.h'),

os.path.join('BulletCollision', 'CollisionDispatch', 'btGhostObject.h'),
# os.path.join('BulletCollision', 'CollisionDispatch', 'btCollisionWorld.cpp'),
# os.path.join('BulletCollision', 'CollisionDispatch', 'btCollisionDispatcher.h'),
# os.path.join('BulletCollision', 'CollisionDispatch', 'btCollisionObject.h'),

# os.path.join('BulletCollision', 'NarrowPhaseCollision', 'btRaycastCallback.h'),
# os.path.join('BulletCollision', 'NarrowPhaseCollision', 'btConvexCast.h'),

# os.path.join('extensions', 'btCharacterController.cpp'),
# os.path.join('extensions', 'btCharacterControllerDefs.h'),
# os.path.join('extensions', 'ccCompoundShape.cpp'),
# os.path.join('extensions', 'ccMaterial.h'),

os.path.join('extensions', 'ccKinematicCharacterController.cpp'),
os.path.join('extensions', 'ccOverlapFilterCallback.h'),
os.path.join('extensions', 'ccRayResultCallback.h'),
os.path.join('extensions', 'ccDiscreteDynamicsWorld.cpp'),
os.path.join('extensions', 'ccConvexCastCallBack.h'),
os.path.join('..', '..', 'idl_templates.h')
]

# Startup

stage_counter = 0

def which(program):
  for path in os.environ["PATH"].split(os.pathsep):
    exe_file = os.path.join(path, program)
    if os.path.exists(exe_file):
      return exe_file
  return None

def build():
  EMSCRIPTEN_ROOT = os.environ.get('EMSCRIPTEN')
  if not EMSCRIPTEN_ROOT:
    envEMSDK = os.environ.get('EMSDK')
    if not envEMSDK:
      print("ERROR: envEMSDK environment variable not found")
      sys.exit(1)
    EMSCRIPTEN_ROOT = os.path.join(envEMSDK, 'upstream', 'emscripten')

  if not EMSCRIPTEN_ROOT:
    print("ERROR: EMSCRIPTEN_ROOT environment variable (which should be equal to emscripten's root dir) not found")
    sys.exit(1)

  sys.path.append(EMSCRIPTEN_ROOT)
  # import tools.config as config
  # import tools.shared as shared
  # Settings

  '''
            Settings.INLINING_LIMIT = 0
            Settings.DOUBLE_MODE = 0
            Settings.PRECISE_I64_MATH = 0
            Settings.CORRECT_SIGNS = 0
            Settings.CORRECT_OVERFLOWS = 0
            Settings.CORRECT_ROUNDINGS = 0
  '''

  wasm = 'wasm' in sys.argv
  closure = 'closure' in sys.argv
  add_function_support = 'add_func' in sys.argv
  full = 'full' in sys.argv
  O1 = 'O1' in sys.argv
  O2 = 'O2' in sys.argv
  O3 = 'O3' in sys.argv
  Os = 'Os' in sys.argv
  Oz = 'Oz' in sys.argv
  g0 = 'g0' in sys.argv
  g1 = 'g1' in sys.argv
  g2 = 'g2' in sys.argv
  g3 = 'g3' in sys.argv
  debug = 'debug' in sys.argv
  release = 'release' in sys.argv

  args = []
  if O1 : args.append('-O1')
  elif O2 : args.append('-O2')
  elif O3 : args.append('-O3')
  elif Os : args.append('-Os')
  elif Oz : args.append('-Oz')
  else:
    not_debug = True

  if g0 : args.append('-g0')
  elif g1 : args.append('-g1')
  elif g2 : args.append('-g2')
  elif g3 : args.append('-g3')
  else:
    not_release = True

  if not_debug and not_release:
      if debug:
        args.append('-g3')
        closure = True
      elif release:
        args.append('-O3')
      else:
        args.append('-O3')
  # --llvm-lto
  args += '--llvm-lto 1 -s EXIT_RUNTIME=0 -s FILESYSTEM=0 -s ASSERTIONS=0 -s -s ENVIRONMENT=web,worker'.split(" ")
  # args = '-O3 --llvm-lto 1 -s NO_EXIT_RUNTIME=1 -s NO_FILESYSTEM=1 -s EXPORTED_RUNTIME_METHODS=["UTF8ToString"] -s ASSERTIONS=1'
  if add_function_support:
    args += '-s ALLOW_TABLE_GROWTH=1 -s EXPORTED_RUNTIME_METHODS=["addFunction"]'.split(" ")
  if not wasm:
    args += '-s WASM=0 -s AGGRESSIVE_VARIABLE_ELIMINATION=1 -s ELIMINATE_DUPLICATE_FUNCTIONS=1 -s SINGLE_FILE=1 -s LEGACY_VM_SUPPORT=1'.split(" ") # -s LEGACY_VM_SUPPORT=1'
  else:
    args += '-s WASM=1 -s BINARYEN_IGNORE_IMPLICIT_TRAPS=1'.split(" ") # -s BINARYEN_TRAP_MODE="clamp"
  if closure:
    args += '--closure 1 -s IGNORE_CLOSURE_COMPILER_ERRORS=1'.split(" ") # closure complains about the bullet Node class (Node is a DOM thing too)
  else:
    args += '-s DYNAMIC_EXECUTION=0'.split(" ")


  emcc_args = args

  emcc_args += ['-s', 'TOTAL_MEMORY=%d' % (64*1024*1024)] # default 64MB. Compile with ALLOW_MEMORY_GROWTH if you want a growable heap (slower though).
  #emcc_args += ['-s', 'ALLOW_MEMORY_GROWTH=1'] # resizable heap, with some amount of slowness

  emcc_args += '-s EXPORT_NAME="Ammo" -s MODULARIZE=1'.split(' ')

  target = 'ammo.js' if not wasm else 'ammo.wasm.js'

  this_idl = 'ammo.release.idl'
  
  if full:
    this_idl = 'ammo.idl'
    target = 'ammo.full.js' if not wasm else 'ammo.full.wasm.js'
  
  if debug:
    this_idl = 'ammo.idl'
    target = 'bullet.debug.asm.js' if not wasm else 'bullet.debug.wasm.js'
  elif release:
    this_idl = 'ammo.release.idl'
    target = 'bullet.release.asm.js' if not wasm else 'bullet.release.wasm.js'

  print('--------------------------------------------------')
  print('Building ammo.js, build type:', emcc_args)
  print('--------------------------------------------------')

  '''
  import os, sys, re

  infile = open(sys.argv[1], 'r').read()
  outfile = open(sys.argv[2], 'w')

  t1 = infile
  while True:
    t2 = re.sub(r'\(\n?!\n?1\n?\+\n?\(\n?!\n?1\n?\+\n?(\w)\n?\)\n?\)', lambda m: '(!1+' + m.group(1) + ')', t1)
    print(len(infile), len(t2))
    if t1 == t2: break
    t1 = t2

  outfile.write(t2)
  '''

  # Utilities

  def stage(text):
    global stage_counter
    stage_counter += 1
    text = 'Stage %d: %s' % (stage_counter, text)
    print('=' * len(text))
    print(text)
    print('=' * len(text))

  # Main

  try:
    this_dir = os.getcwd()
    os.chdir('bullet')
    if not os.path.exists('build'):
      os.makedirs('build')
    os.chdir('build')

    glue_args = ["python",os.path.join(EMSCRIPTEN_ROOT, 'tools', 'webidl_binder.py'), os.path.join(this_dir, this_idl), 'glue']
    stage('Generate bindings:'+' '.join(glue_args))
    Popen(glue_args).communicate()
    # Popen([emscripten.PYTHON, os.path.join(EMSCRIPTEN_ROOT, 'tools', 'webidl_binder.py'), os.path.join(this_dir, 'ammo.idl'), 'glue']).communicate()
    assert os.path.exists('glue.js')
    assert os.path.exists('glue.cpp')

    args = ['-I../src', '-c']
    for include in INCLUDES:
      args += ['-include', include]
    args = ['emcc','glue.cpp'] + args
    args = args + ['-o','glue.o']

    stage('Build bindings:'+' '.join(args))

    Popen(args).communicate()
    assert(os.path.exists('glue.o'))

    # Configure with CMake on Windows, and with configure on Unix.
    cmake_build = sys.platform.startswith("win")

    if cmake_build:
      if not os.path.exists('CMakeCache.txt'):
        stage('Configure via CMake')
        Popen(['python', os.path.join(EMSCRIPTEN_ROOT, 'emcmake'), 'cmake', '..', '-DBUILD_DEMOS=OFF', '-DBUILD_EXTRAS=OFF', '-DBUILD_CPU_DEMOS=OFF', '-DUSE_GLUT=OFF', '-DCMAKE_BUILD_TYPE=Release']).communicate()
    else:
      if not os.path.exists('config.h'):
        stage('Configure (if this fails, run autogen.sh in bullet/ first)')
        Popen(['emconfigure','../configure', '--disable-demos','--disable-dependency-tracking']).communicate()

    stage('Make')

    CORES = multiprocessing.cpu_count()

    if cmake_build:
      Popen(['mingw32-make', '-j', str(CORES)]).communicate()
    else:
      p = Popen(['emmake','make', '-j', str(CORES)], stdout=PIPE, stderr=PIPE)
      stdout, stderr = p.communicate()
      if stderr:
          print(f'Error: {stderr.decode()}')

    # Popen(['emcc', '-o',"mylib.bc *.o"]).communicate()

    stage('Link')

    if cmake_build:
      bullet_libs = [os.path.join('src', 'BulletSoftBody', 'libBulletSoftBody.a'),
                    os.path.join('src', 'BulletDynamics', 'libBulletDynamics.a'),
                    os.path.join('src', 'BulletCollision', 'libBulletCollision.a'),
                    os.path.join('src', 'LinearMath', 'libLinearMath.a')]
    else:
      bullet_libs = [os.path.join('src', '.libs', 'libBulletSoftBody.a'),
                    os.path.join('src', '.libs', 'libBulletDynamics.a'),
                    os.path.join('src', '.libs', 'libBulletCollision.a'),
                    os.path.join('src', '.libs', 'libLinearMath.a')]

    temp = os.path.join('..','..', 'builds', target)
    print('temp: ' + temp)
    if os.path.exists(temp):
      os.remove(temp)
    args = ['emcc','-DNOTHING_WAKA_WAKA']
    args = args  + ['glue.o']+ bullet_libs + emcc_args#  + ['--js-transform', 'python %s' % os.path.join('..','..', 'bundle.py')]
    args = args + ['-o',temp]
    Popen(["pwd"]).communicate()
    print('emcc: ' + ' '.join(args))
    p = Popen(args, stdout=PIPE, stderr=PIPE)
    stdout, stderr = p.communicate()
    if stderr:
        print(f'Error: {stderr.decode()}')

    assert os.path.exists(temp), 'Failed to create script code'

    stage('wrap')

    wrapped = '''
  // This is ammo.js, a port of Bullet Physics to JavaScript. zlib licensed.
  ''' + open(temp).read()

    open(temp, 'w').write(wrapped)

  except Exception as e:
    print(f"发生错误：{e}")
  finally:
    os.chdir(this_dir);
    sys.exit()

if __name__ == '__main__':
  build()

