from setuptools import setup
from glob import glob

package_name = 'joint_analyzer'

setup(
    name=package_name,
    version='0.0.1',
    packages=[package_name, package_name + '.templates', package_name + '.static'],
    data_files=[
        ('share/ament_index/resource_index/packages',
         ['resource/' + package_name]),

        ('share/' + package_name,
         ['package.xml']),

        ('share/' + package_name + '/launch',
         glob('launch/*.launch.py')),
    ],
    package_data={
        package_name: [
            'templates/*.html',
            'static/*.js',
            'static/*.css',
        ],
    },
    include_package_data=True,
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='baranidharan',
    maintainer_email='selvambharani100@gmail.com',
    description='Web tool for comparing commanded vs actual joint positions, and a generic live topic-field plotter, over Flask.',
    license='Apache-2.0',
    entry_points={
        'console_scripts': [
            'server = joint_analyzer.server:main',
            'plot_server = joint_analyzer.plot_server:main',
            'check_diffs = joint_analyzer.check_diffs:main',
            'diagnose_qos = joint_analyzer.diagnose_qos:main',
            'verify_mapping = joint_analyzer.verify_mapping:main',
        ],
    },
)
